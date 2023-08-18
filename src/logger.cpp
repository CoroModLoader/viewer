#include "logger.hpp"

#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace explorer
{
    struct logger::impl
    {
        std::shared_ptr<spdlog::logger> logger;
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("explorer.log");
        auto ansi_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();

        m_impl->logger = std::make_shared<spdlog::logger>("explorer", spdlog::sinks_init_list{file_sink, ansi_sink});
        m_impl->logger->should_log(spdlog::level::trace);
    }

    logger::~logger() = default;

    spdlog::logger *logger::operator->() const
    {
        return m_impl->logger.get();
    }

    logger &logger::get()
    {
        static std::unique_ptr<logger> instance;

        if (!instance)
        {
            instance = std::unique_ptr<logger>(new logger);
        }

        return *instance;
    }
} // namespace explorer