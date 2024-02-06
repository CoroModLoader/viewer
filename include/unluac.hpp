#pragma once

#include <string>
#include <memory>

#include <optional>
#include <filesystem>
#include <tl/expected.hpp>

namespace viewer
{
    namespace fs = std::filesystem;

    enum class decomp_error_type
    {
        start,
        read,
        unknown,
    };

    struct decomp_error
    {
        std::string message;
        decomp_error_type type;
    };

    class unluac
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~unluac();

      public:
        unluac(const std::optional<std::string> &java, const std::optional<std::string> &unluac);

      public:
        [[nodiscard]] tl::expected<std::string, decomp_error> decompile(const fs::path &) const;
    };
} // namespace viewer
