#pragma once

#include <vector>
#include <string>
#include <memory>

#include <optional>
#include <filesystem>
#include <tl/expected.hpp>

namespace solar2d
{
    namespace fs = std::filesystem;

    enum class archive_error
    {
        bad_file,
        no_header,
    };

    enum class resource_type
    {
        lua_resource
    };

    struct file
    {
        friend class archive;

      public:
        std::string name;
        resource_type type;

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
        archive(const archive &);
        archive(archive &&) noexcept;

      public:
        [[nodiscard]] std::vector<file> files() const;
        [[nodiscard]] std::vector<char> get_data(const file &file);
        [[nodiscard]] std::optional<file> get(const std::string &name) const;

      public:
        void extract(const file &file, const fs::path &dest);

      public:
        static tl::expected<archive, archive_error> from(const fs::path &path);
    };
} // namespace solar2d
