#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace viewer
{
    struct logger::impl
    {
        std::unique_ptr<spdlog::logger> logger;
    };

    logger::logger() : m_impl(std::make_unique<impl>())
    {
        namespace sinks = spdlog::sinks;

        auto stdout_sink = std::make_shared<sinks::ansicolor_stdout_sink_mt>();
        stdout_sink->set_level(spdlog::level::debug);

        auto file_sink = std::make_shared<sinks::basic_file_sink_mt>("coroviewer.log");
        file_sink->set_level(spdlog::level::trace);

        auto init      = spdlog::sinks_init_list{stdout_sink, file_sink};
        m_impl->logger = std::make_unique<spdlog::logger>("coroviewer", init);

        m_impl->logger->set_level(spdlog::level::trace);
        m_impl->logger->flush_on(spdlog::level::trace);
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
