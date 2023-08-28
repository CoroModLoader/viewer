#include "logger.hpp"
#include "unluac.hpp"
#include "archive.hpp"

#include <nfd.hpp>
#include <filesystem>
#include <saucer/smartview.hpp>

#include "embedding/all.hpp"

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

    NFD_Init();
    saucer::smartview saucer({.persistent_cookies = true, .hardware_acceleration = true});

    saucer.set_title("Solar2D Viewer");
    saucer.set_min_size(800, 600);

    std::optional<solar2d::archive> archive;

    saucer.expose("open-archive", [&]() {
        nfdchar_t *path{};
        nfdfilteritem_t filters[1] = {{"Car Archive", "car"}};
        nfdresult_t result = NFD_OpenDialog(&path, filters, 1, nullptr);

        if (result != NFD_OKAY)
        {
            return false;
        }

        auto rtn = solar2d::archive::from(path);

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
        auto temp = std::filesystem::temp_directory_path() / "solar2d-dec.lua";
        auto target = archive->get(file);

        if (!target)
        {
            logger::get()->error("could not find requested file \"{}\"", file);
            return std::nullopt;
        }

        archive->extract(target.value(), temp);
        auto rtn = viewer::unluac::get().decompile(temp);

        if (!rtn)
        {
            return std::nullopt;
        }

        return rtn.value();
    });

    saucer.expose("java-path", []() { return viewer::unluac::get().java_path(); });
    saucer.expose("update-java-path", []() -> std::string {
        nfdchar_t *path{};
        nfdresult_t result = NFD_OpenDialog(&path, nullptr, 0, nullptr);

        if (result != NFD_OKAY)
        {
            return viewer::unluac::get().java_path();
        }

        viewer::unluac::get().set_java_path(path);
        return path;
    });

    saucer.expose("unluac-path", []() { return viewer::unluac::get().unluac_path(); });
    saucer.expose("update-unluac-path", []() -> std::string {
        nfdchar_t *path{};
        nfdfilteritem_t filters[1] = {{"Jar File", "jar"}};
        nfdresult_t result = NFD_OpenDialog(&path, filters, 1, nullptr);

        if (result != NFD_OKAY)
        {
            return viewer::unluac::get().java_path();
        }

        viewer::unluac::get().set_unluac_path(path);
        return path;
    });

    saucer.embed(embedded::get_all_files());
    saucer.set_context_menu(false);
    saucer.serve("index.html");
    saucer.show();
    saucer.run();

    return 0;
}