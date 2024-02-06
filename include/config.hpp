#pragma once

#include <string>
#include <optional>
#include <filesystem>

namespace viewer
{
    namespace fs = std::filesystem;

    struct config
    {
        std::optional<std::string> java_path;
        std::optional<std::string> unluac_path;

      public:
        void save();

      public:
        static config &load();

      private:
        [[nodiscard]] static fs::path path();
    };
} // namespace viewer
