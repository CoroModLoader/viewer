#include "config.hpp"
#include "logger.hpp"

#include <filesystem>
#include <glaze/glaze.hpp>
#include <memory>

namespace viewer
{
    void config::save()
    {
        auto cfg    = path();
        auto parent = cfg.parent_path();

        if (!fs::exists(parent))
        {
            fs::create_directories(parent);
        }

        std::ofstream output{cfg, std::ios::trunc};
        auto data = glz::write_json(*this);

        output << data;
    }

    config &config::load()
    {
        static std::unique_ptr<config> instance;

        if (!instance)
        {
            instance = std::make_unique<config>();

            auto cfg = path();

            if (!fs::exists(cfg) || !fs::is_regular_file(cfg))
            {
                logger::get()->warn("bad config: '{}'", cfg.string());
                return *instance;
            }

            std::ifstream stream{cfg};
            std::string content{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};

            auto parsed = glz::read_json<config>(content);

            if (!parsed.has_value())
            {
                logger::get()->warn("bad config: {} ('{}')", static_cast<std::uint32_t>(parsed.error().ec),
                                    cfg.string());

                return *instance;
            }

            instance->java_path   = parsed->java_path;
            instance->unluac_path = parsed->unluac_path;
        }

        return *instance;
    }

    fs::path config::path()
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        auto *home = std::getenv("HOME");

        if (home)
        {
            return fs::path(home) / ".config" / "coroviewer" / "config.json";
        }

        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        home = std::getenv("APPDATA");

        if (home)
        {
            return fs::path(home) / "coroviewer" / "config.json";
        }

        logger::get()->warn("can't determine config location!");

        return fs::current_path() / "config.json";
    }

} // namespace viewer
