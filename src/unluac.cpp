#include "unluac.hpp"
#include "logger.hpp"

#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>

namespace viewer
{
    struct unluac::impl
    {
        std::string java;
        std::string unluac;
    };

    unluac::~unluac() = default;

    unluac::unluac(unluac &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    unluac::unluac(std::string java, std::string unluac) : m_impl(std::make_unique<impl>())
    {
        m_impl->java = std::move(java);
        m_impl->unluac = std::move(unluac);
    }

    unluac &unluac::operator=(unluac &&other) noexcept
    {
        if (this != &other)
        {
            m_impl = std::move(other.m_impl);
        }

        return *this;
    }

    tl::expected<std::string, decompile_error> unluac::decompile(const fs::path &file) const
    {
        reproc::process proc;

        std::vector<std::string> arguments{m_impl->java, "-jar", m_impl->unluac, file.string()};
        auto error = proc.start(arguments);

        if (error)
        {
            logger::get()->error("failed to start unluac: {} ({})", error.message(), error.value());
            return tl::make_unexpected(decompile_error::start);
        }

        std::string rtn;
        reproc::sink::string sink(rtn);

        error = reproc::drain(proc, sink, reproc::sink::null);

        if (error)
        {
            logger::get()->error("failed to read output: {} ({})", error.message(), error.value());
            return tl::make_unexpected(decompile_error::read);
        }

        int status{0};
        std::tie(status, error) = proc.wait(reproc::infinite);

        if (status != 0)
        {
            logger::get()->error("unluac failed: {} ({}) [{}]", error.message(), error.value(), status);
            return tl::make_unexpected(decompile_error::unknown);
        }

        return rtn;
    }
} // namespace viewer
