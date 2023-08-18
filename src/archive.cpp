#include "archive.hpp"
#include "archive.impl.hpp"

#include "logger.hpp"

#include <fstream>
#include <cstring>
#include <filesystem>

namespace solar2d
{
    archive::archive() : m_impl(std::make_unique<impl>()) {}

    archive::~archive()
    {
        if (!m_impl)
        {
            return;
        }

        m_impl->file.close();
    }

    archive::archive(archive &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    std::vector<file> archive::files() const
    {
        auto tag = m_impl->read<enum tag>();

        if (tag != tag::content)
        {
            return {};
        }

        auto count = m_impl->read();

        std::vector<file> rtn(count);

        for (auto i = 0u; count > i; i++)
        {
            file entry;

            entry.type = static_cast<type>(m_impl->read());
            entry.offset = m_impl->read();
            entry.name = m_impl->read<std::string>();

            rtn.emplace_back(entry);
        }

        return rtn;
    }

    std::vector<char> archive::data(const file &file)
    {
        m_impl->file.seekg(static_cast<std::streamoff>(file.offset));

        auto tag = m_impl->read<enum tag>();

        if (tag != tag::data)
        {
            return {};
        }

        return m_impl->read<std::vector<char>>();
    }

    void archive::extract(const file &file, const fs::path &dest)
    {
        using explorer::logger;

        if (!fs::exists(dest))
        {
            fs::create_directories(dest);
        }

        if (!fs::is_directory(dest))
        {
            logger::get()->error("'{}' is not a directory", dest.string());
            return;
        }

        auto output_path = dest / file.name;
        std::ofstream output(output_path);

        auto buf = data(file);

        output.write(buf.data(), static_cast<std::streamsize>(buf.size()));
        output.close();

        logger::get()->debug("Extracted '{}' to '{}'", file.name, output_path.string());
    }

    std::optional<archive> archive::from(const fs::path &path)
    {
        using explorer::logger;

        if (!fs::exists(path) || !fs::is_regular_file(path))
        {
            logger::get()->error("'{}' does not exist or is not a file", path.string());
            return std::nullopt;
        }

        static constexpr std::uint8_t header[] = {'r', 'a', 'c', version};
        static constexpr auto header_size = sizeof(header);

        std::ifstream file{path, std::ios::binary};
        file.unsetf(std::ios::skipws);

        char buf[header_size];
        file.read(buf, header_size);

        if (memcmp(buf, header, header_size) != 0)
        {
            logger::get()->error("'{}' is missing car header", path.string());
            return std::nullopt;
        }

        archive rtn;
        rtn.m_impl->file = std::move(file);

        return rtn;
    }
} // namespace solar2d