#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace viewer
{
    class logger
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        logger();

      public:
        ~logger();

      public:
        spdlog::logger *operator->() const;

      public:
        static logger &get();
    };
} // namespace viewer