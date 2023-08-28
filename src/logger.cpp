#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace viewer
{
    struct logger::impl
    {
        std::shared_ptr<spdlog::logger> logger;
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("viewer.log");
        auto color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        m_impl->logger = std::make_shared<spdlog::logger>("viewer", spdlog::sinks_init_list{file_sink, color_sink});
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
} // namespace viewer