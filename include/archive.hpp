#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <filesystem>

namespace solar2d
{
    namespace fs = std::filesystem;

    enum class type
    {
        lua_resource
    };

    struct file
    {
        friend class archive;

      public:
        type type;
        std::string name;

      private:
        std::size_t offset;
    };

    class archive
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        archive();

      public:
        ~archive();

      public:
        archive(archive &&) noexcept;

      public:
        [[nodiscard]] std::vector<file> files() const;

      public:
        std::vector<char> data(const file &file);
        void extract(const file &file, const fs::path &dest);

      public:
        static std::optional<archive> from(const fs::path &path);

      public:
        static constexpr auto version = 0x1;
    };
} // namespace solar2d