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

    unluac::unluac(const std::optional<std::string> &java, const std::optional<std::string> &unluac)
        : m_impl(std::make_unique<impl>())
    {
        m_impl->java   = java.value_or("java");
        m_impl->unluac = unluac.value_or("unluac.jar");
    }

    tl::expected<std::string, decomp_error> unluac::decompile(const fs::path &file) const
    {
        reproc::process proc;

        std::vector<std::string> arguments{m_impl->java, "-jar", m_impl->unluac, file.string()};
        auto error = proc.start(arguments, {.redirect = {.err = {reproc::redirect::pipe}}});

        if (error)
        {
            logger::get()->error("failed to start unluac: {} ({})", error.message(), error.value());
            return tl::make_unexpected(decomp_error{error.message(), decomp_error_type::start});
        }

        std::string rtn;

        reproc::sink::string sink(rtn);
        error = reproc::drain(proc, sink, sink);

        if (error)
        {
            logger::get()->error("failed to read output: {} ({})", error.message(), error.value());
            return tl::make_unexpected(decomp_error{"<Unknown>", decomp_error_type::read});
        }

        int status{0};
        std::tie(status, error) = proc.wait(reproc::infinite);

        if (status != 0)
        {
            logger::get()->error("unluac failed: {} ({}) [{}]", error.message(), error.value(), status);
            return tl::make_unexpected(decomp_error{rtn, decomp_error_type::unknown});
        }

        return rtn;
    }
} // namespace viewer
