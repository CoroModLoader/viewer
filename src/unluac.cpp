#include "unluac.hpp"
#include "logger.hpp"

#include <fstream>
#include <filesystem>

#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>

namespace viewer
{
    struct unluac::impl
    {
        std::string java_path;
        std::string unluac_path;

      public:
        void save() const;
        static fs::path config_path();
    };

    void unluac::impl::save() const
    {
        auto path = config_path();

        if (!fs::exists(path))
        {
            fs::create_directories(path.parent_path());
        }

        std::ofstream file{path};

        nlohmann::json config;

        config["java_path"] = java_path;
        config["unluac_path"] = unluac_path;

        file << config.dump();
    }

    fs::path unluac::impl::config_path()
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        auto *home = std::getenv("HOME");

        if (home)
        {
            return fs::path(home) / ".config" / "solar2d-viewer" / "config.json";
        }

        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        home = std::getenv("APPDATA");

        if (home)
        {
            return fs::path(home) / "solar2d-viewer" / "config.json";
        }

        logger::get()->warn("can't determine directory to place config file");

        return fs::current_path() / "config.json";
    }

    unluac::unluac() : m_impl(std::make_unique<impl>())
    {
        auto config = impl::config_path();

        logger::get()->info("config path: {}", config.string());

        m_impl->java_path = "java";
        m_impl->unluac_path = fs::current_path() / "unluac.jar";

        if (!fs::exists(config) || !fs::is_regular_file(config))
        {
            return;
        }

        std::fstream file{config};
        auto parsed = nlohmann::json::parse(file, nullptr, false);

        if (parsed.is_discarded())
        {
            logger::get()->error("failed to read config");
            return;
        }

        m_impl->java_path = parsed["java_path"];
        m_impl->unluac_path = parsed["unluac_path"];
    }

    unluac::~unluac() = default;

    std::string unluac::java_path() const
    {
        return m_impl->java_path;
    }

    std::string unluac::unluac_path() const
    {
        return m_impl->unluac_path;
    }

    void unluac::set_java_path(std::string path)
    {
        m_impl->java_path = std::move(path);
        m_impl->save();
    }

    void unluac::set_unluac_path(std::string path)
    {
        m_impl->unluac_path = std::move(path);
        m_impl->save();
    }

    std::optional<std::string> unluac::decompile(const fs::path &file) const
    {
        reproc::process proc;

        std::vector<std::string> arguments{m_impl->java_path, "-jar", m_impl->unluac_path, file.string()};
        auto error = proc.start(arguments);

        if (error)
        {
            logger::get()->error("failed to start unluac: {} ({})", error.message(), error.value());
            return std::nullopt;
        }

        std::string rtn;
        reproc::sink::string sink(rtn);

        error = reproc::drain(proc, sink, reproc::sink::null);

        if (error)
        {
            logger::get()->error("failed to drain unluac: {} ({})", error.message(), error.value());
            return std::nullopt;
        }

        int status{0};
        std::tie(status, error) = proc.wait(reproc::infinite);

        if (status != 0)
        {
            logger::get()->error("failed to decompile: {} ({}) [{}]", error.message(), error.value(), status);
            return std::nullopt;
        }

        return rtn;
    }

    unluac &unluac::get()
    {
        static std::unique_ptr<unluac> instance;

        if (!instance)
        {
            instance = std::unique_ptr<unluac>(new unluac);
        }

        return *instance;
    }
} // namespace viewer