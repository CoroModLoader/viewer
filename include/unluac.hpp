#pragma once
#include <string>
#include <memory>
#include <optional>
#include <filesystem>

namespace viewer
{
    namespace fs = std::filesystem;

    class unluac
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        unluac();

      public:
        ~unluac();

      public:
        [[nodiscard]] std::string java_path() const;
        [[nodiscard]] std::string unluac_path() const;

      public:
        void set_java_path(std::string);
        void set_unluac_path(std::string);

      public:
        [[nodiscard]] std::optional<std::string> decompile(const fs::path &) const;

      public:
        static unluac &get();
    };
} // namespace viewer