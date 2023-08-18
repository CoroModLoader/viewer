#pragma once
#include "archive.hpp"

#include <fstream>

namespace solar2d
{
    struct archive::impl
    {
        std::ifstream file;

      public:
        template <typename T = std::uint32_t>
        T read();
    };

    enum class tag : unsigned int
    {
        unknown,
        content,
        data,

        eof = 0xFFFFFFFF
    };

    template <>
    std::vector<char> archive::impl::read<std::vector<char>>();

    template <>
    std::uint32_t archive::impl::read<std::uint32_t>();

    template <>
    std::string archive::impl::read<std::string>();

    template <>
    tag archive::impl::read<tag>();
}; // namespace solar2d