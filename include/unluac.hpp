#pragma once

#include <string>
#include <memory>

#include <filesystem>
#include <tl/expected.hpp>

namespace viewer
{
    namespace fs = std::filesystem;

    enum class decompile_error
    {
        start,
        read,
        unknown,
    };

    class unluac
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~unluac();

      public:
        unluac(unluac &&) noexcept;
        unluac(std::string java = "java", std::string unluac = "unluac.jar");

      public:
        unluac &operator=(unluac &&) noexcept;

      public:
        [[nodiscard]] tl::expected<std::string, decompile_error> decompile(const fs::path &) const;
    };
} // namespace viewer
