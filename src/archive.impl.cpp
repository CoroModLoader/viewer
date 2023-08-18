#include "archive.impl.hpp"

namespace solar2d
{
    template <std::size_t N>
    std::ifstream::pos_type byte_aligned(std::size_t value)
    {
        return (value + (N - 1)) & (~(N - 1));
    }

    template <>
    std::vector<char> archive::impl::read<std::vector<char>>()
    {
        const auto len = read();
        const auto pos = file.tellg();

        std::vector<char> rtn(len);

        file.read(rtn.data(), len);
        file.seekg(pos + byte_aligned<4>(len));

        return rtn;
    }

    template <>
    std::uint32_t archive::impl::read<std::uint32_t>()
    {
        std::uint32_t rtn{};
        file.read(reinterpret_cast<char *>(&rtn), sizeof(std::uint32_t));

        return rtn;
    }

    template <>
    std::string archive::impl::read<std::string>()
    {
        const auto len = read();
        const auto pos = file.tellg();

        std::string rtn;
        rtn.resize(len);

        file.read(rtn.data(), len);
        file.seekg(pos + byte_aligned<4>(len + 1));

        return rtn;
    }

    template <>
    tag archive::impl::read<enum tag>()
    {
        const auto tag = static_cast<enum tag>(read());
        [[maybe_unused]] const auto len = read();

        return tag;
    }
} // namespace solar2d