#include "controller.hpp"

#include "config.hpp"
#include "unluac.hpp"
#include "logger.hpp"

#include "archive.hpp"

#include <cmath>

#include <thread>
#include <ranges>
#include <vector>

#include <fstream>
#include <fmt/core.h>

namespace viewer
{
    namespace fs = std::filesystem;

    struct controller::impl
    {
        std::vector<std::jthread> pool;
        std::optional<solar2d::archive> archive;
    };

    controller::~controller() = default;

    controller::controller() : m_impl(std::make_unique<impl>()) {}

    tl::expected<void, std::string> controller::load(const std::string &path)
    {
        m_impl->archive.reset();

        auto archive = solar2d::archive::from(path);

        if (!archive.has_value())
        {
            auto error = archive.error();

            switch (error)
            {
            case solar2d::archive_error::no_header:
                return tl::make_unexpected("Archive is missing CAR header");
            case solar2d::archive_error::bad_file:
                return tl::make_unexpected("The supplied file is bad");
            default:
                return tl::make_unexpected("Unknown Error");
            }
        }

        m_impl->archive.emplace(std::move(archive.value()));

        return {};
    }

    std::vector<std::string> controller::list()
    {
        if (!m_impl->archive)
        {
            return {};
        }

        auto to_name = [](const auto &file)
        {
            return file.name;
        };

        auto rtn = m_impl->archive->files() | std::views::transform(to_name);
        return {rtn.begin(), rtn.end()};
    }

    std::string controller::decompile(const std::string &name)
    {
        static const auto error = []<typename... T>(fmt::format_string<T...> format, T &&...args)
        {
            return fmt::format("[[\n\tDecompilation Error!\n\t{}]]", fmt::format(format, std::forward<T>(args)...));
        };

        if (!m_impl->archive)
        {
            return error("no archive loaded");
        }

        auto files = m_impl->archive->files();
        auto file  = std::ranges::find_if(files, [&](const auto &item) { return item.name == name; });

        if (file == files.end())
        {
            return error("requested file does not exist");
        }

        auto path = fs::temp_directory_path() / "coroviewer" / name;

        if (!fs::exists(path))
        {
            m_impl->archive->extract(*file, path);
        }

        auto &config = viewer::config::load();
        viewer::unluac decomp{config.java_path, config.unluac_path};

        auto source = decomp.decompile(path);

        if (!source.has_value())
        {
            auto err = source.error();

            switch (err.type)
            {
            case viewer::decomp_error_type::read:
                return error("[failed to read from unluac]: {}", err.message);
            case viewer::decomp_error_type::start:
                return error("[failed to invoke unluac]: {}", err.message);
            case viewer::decomp_error_type::unknown:
            default:
                return error("[unknown error]: {}", err.message);
            }
        }

        return source.value();
    }

    tl::expected<std::stop_source, std::string> controller::export_archive(const fs::path &path,
                                                                           std::atomic_size_t &counter)
    {
        if (!m_impl->archive)
        {
            return tl::make_unexpected("no archive loaded");
        }

        if (!fs::exists(path))
        {
            fs::create_directories(path);
        }

        if (!fs::is_directory(path))
        {
            return tl::make_unexpected("specified path is not a directory");
        }

        auto raw_path    = path / "raw";
        auto decomp_path = path / "decompiled";

        fs::create_directories(raw_path);
        fs::create_directories(decomp_path);

        m_impl->pool.clear();

        const auto concurrency = std::thread::hardware_concurrency();
        m_impl->pool.reserve(concurrency);

        auto rtn = std::stop_source{};

        const auto total = m_impl->archive->files().size();
        const auto slice = total / concurrency;

        for (auto i = 0u; concurrency > i; i++)
        {
            const auto start = static_cast<std::size_t>(std::floor(slice * i));
            const auto end   = (i == concurrency - 1) ? total : start + slice;

            auto thread = [start, end, path, raw_path, decomp_path, &counter](auto id, auto archive, auto token)
            {
                logger::get()->debug("[Thread {:02}] working on slice {}-{}", id, start, end);

                const auto &config = config::load();
                auto decomp        = unluac{config.java_path, config.unluac_path};

                const auto files = archive.files();

                for (auto i = start; end > i; i++, counter++)
                {
                    if (token.stop_requested())
                    {
                        break;
                    }

                    const auto &file = files[i];

                    const auto raw        = raw_path / file.name;
                    const auto decompiled = decomp_path / fmt::format("{}.lua", file.name);

                    archive.extract(file, raw);

                    const auto source = decomp.decompile(raw);

                    if (!source.has_value())
                    {
                        logger::get()->warn("failed to decompile: '{}'", raw.string());
                        continue;
                    }

                    std::ofstream output{decompiled};
                    output << source.value();
                }

                logger::get()->debug("[Thread {:02}] finished", id);
            };

            m_impl->pool.emplace_back(thread, i, m_impl->archive.value(), rtn.get_token());
        }

        return rtn;
    }
} // namespace viewer
