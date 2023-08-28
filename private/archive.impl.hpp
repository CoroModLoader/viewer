#pragma once
#include "archive.hpp"

#include <fstream>
#include <cstdint>

namespace solar2d
{
    struct archive::impl
    {
        std::ifstream file;
        std::ifstream::pos_type start;

      public:
        template <typename T = std::uint32_t>
        T read();

      public:
        static constexpr auto version = 0x1;
    };

    enum class tag : unsigned int
    {
        unknown,
        content,
        data,

        eof = 0xFFFFFFFF
    };

    template <>
    std::vector<std::uint8_t> archive::impl::read<std::vector<std::uint8_t>>();

    template <>
    std::uint32_t archive::impl::read<std::uint32_t>();

    template <>
    std::string archive::impl::read<std::string>();

    template <>
    tag archive::impl::read<tag>();
}; // namespace solar2d