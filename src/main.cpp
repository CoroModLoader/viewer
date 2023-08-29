#include "logger.hpp"
#include "unluac.hpp"
#include "archive.hpp"

#include <map>
#include <fstream>
#include <filesystem>

#include <nfd.hpp>
#include <saucer/smartview.hpp>
#include <saucer/utils/future.hpp>

#include "embedding/all.hpp"

namespace fs = std::filesystem;

namespace nlohmann
{
    template <typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void to_json(json &json, const std::optional<T> &value)
        {
            if (!value)
            {
                json = nullptr;
                return;
            }

            json = value.value();
        }

        static void from_json(const json &j, std::optional<T> &value)
        {
            if (j.is_null())
            {
                return;
            }

            value.emplace(j);
        }
    };
} // namespace nlohmann

int main()
{
    using viewer::logger;
    using viewer::unluac;

    NFD_Init();
    saucer::smartview saucer({.persistent_cookies = true, .hardware_acceleration = true});

    saucer.set_title("Solar2D Viewer");
    saucer.set_min_size(800, 600);

    std::optional<solar2d::archive> archive;
    std::map<std::string, std::string> cache;

    saucer.expose("open-archive", [&]() {
        NFD::UniquePath path;
        nfdfilteritem_t filters[1] = {{"Car Archive", "car"}};
        auto result = NFD::OpenDialog(path, filters, 1);

        if (result != NFD_OKAY)
        {
            return false;
        }

        auto rtn = solar2d::archive::from(path.get());

        if (!rtn)
        {
            return false;
        }

        archive.emplace(std::move(*rtn));

        return true;
    });

    saucer.expose("list-files", [&]() {
        std::vector<std::string> rtn;

        for (const auto &file : archive->files())
        {
            rtn.emplace_back(file.name);
        }

        return rtn;
    });

    saucer.expose("decompile", [&](const std::string &file) -> std::optional<std::string> {
        if (cache.contains(file))
        {
            return cache.at(file);
        }

        auto temp = fs::temp_directory_path() / "solar2d-dec.lua";
        auto target = archive->get(file);

        if (!target)
        {
            logger::get()->error("could not find requested file \"{}\"", file);
            return std::nullopt;
        }

        archive->extract(target.value(), temp);
        auto rtn = unluac::get().decompile(temp);

        if (!rtn)
        {
            return std::nullopt;
        }

        cache.emplace(file, rtn.value());

        return rtn.value();
    });

    saucer.expose("java-path", []() { return unluac::get().java_path(); });
    saucer.expose("update-java-path", []() -> std::string {
        NFD::UniquePath path;
        auto result = NFD::OpenDialog(path);

        if (result != NFD_OKAY)
        {
            return unluac::get().java_path();
        }

        unluac::get().set_java_path(path.get());

        return path.get();
    });

    saucer.expose("unluac-path", []() { return unluac::get().unluac_path(); });
    saucer.expose("update-unluac-path", []() -> std::string {
        NFD::UniquePath path;
        nfdfilteritem_t filters[1] = {{"Jar File", "jar"}};
        auto result = NFD::OpenDialog(path, filters, 1);

        if (result != NFD_OKAY)
        {
            return unluac::get().java_path();
        }

        unluac::get().set_unluac_path(path.get());
        return path.get();
    });

    saucer.expose(
        "export-all",
        [&]() {
            NFD::UniquePath path;
            auto result = NFD::PickFolder(path);

            if (result != NFD_OKAY)
            {
                return;
            }

            auto temp = fs::temp_directory_path() / "solar2d-export";
            auto copy = archive.value();

            auto files = copy.files();

            for (const auto &file : files)
            {
                copy.extract(file, temp);
            }

            auto out_dir = fs::path(path.get());
            auto &unluac = unluac::get();
            auto extracted = 0u;

            for (const auto &file : files)
            {
                if (extracted % 10 == 0 || extracted == files.size())
                {
                    saucer::forget(saucer.evaluate<void>("window.update_notification({})",
                                                         saucer::make_args(extracted, files.size())));
                }

                auto lua = unluac.decompile(temp / file.name);

                if (!lua)
                {
                    continue;
                }

                std::ofstream out{out_dir / (file.name + ".lua"), std::ios::out};
                out << lua.value();
                extracted++;
            }
        },
        true);

    saucer.embed(embedded::get_all_files());
    saucer.set_context_menu(false);
    saucer.serve("index.html");
    saucer.show();
    saucer.run();

    return 0;
}