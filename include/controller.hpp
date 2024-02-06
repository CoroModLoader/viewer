#pragma once

#include <memory>
#include <vector>

#include <stop_token>
#include <filesystem>

#include <tl/expected.hpp>

namespace viewer
{
    namespace fs = std::filesystem;

    class controller
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~controller();

      public:
        controller();

      public:
        tl::expected<void, std::string> load(const std::string &path);

      public:
        std::vector<std::string> list();
        std::string decompile(const std::string &name);

      public:
        tl::expected<std::stop_source, std::string> export_archive(const fs::path &path, std::atomic_size_t &counter);
    };
} // namespace viewer
