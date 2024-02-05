#include "archive.hpp"
#include "archive.impl.hpp"

#include "logger.hpp"

#include <fstream>
#include <cstring>
#include <filesystem>

namespace solar2d
{
    using viewer::logger;

    archive::archive() : m_impl(std::make_unique<impl>()) {}

    archive::~archive() = default;

    archive::archive(archive &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    archive::archive(const archive &other) : m_impl(std::make_unique<impl>())
    {
        m_impl->file = std::ifstream{other.m_impl->location, std::ios::binary};
        m_impl->file.unsetf(std::ios::skipws);
        m_impl->file.seekg(other.m_impl->start);

        m_impl->location = other.m_impl->location;
        m_impl->start = other.m_impl->start;
    }

    std::vector<file> archive::files() const
    {
        m_impl->file.seekg(m_impl->start);

        auto tag = m_impl->read<enum tag>();

        if (tag != tag::content)
        {
            return {};
        }

        auto count = m_impl->read();

        std::vector<file> rtn;
        rtn.reserve(count);

        for (auto i = 0u; count > i; i++)
        {
            file entry;

            entry.type = static_cast<resource_type>(m_impl->read());
            entry.offset = m_impl->read();
            entry.name = m_impl->read<std::string>();

            rtn.emplace_back(entry);
        }

        return rtn;
    }

    std::vector<char> archive::get_data(const file &file)
    {
        m_impl->file.seekg(static_cast<std::streamoff>(file.offset));

        auto tag = m_impl->read<enum tag>();

        if (tag != tag::data)
        {
            return {};
        }

        return m_impl->read<std::vector<char>>();
    }

    std::optional<file> archive::get(const std::string &name) const
    {
        auto files = this->files();
        auto it = std::ranges::find_if(files, [&](auto &x) { return x.name == name; });

        if (it == files.end())
        {
            return std::nullopt;
        }

        return *it;
    }

    void archive::extract(const file &file, const fs::path &dest)
    {
        auto folder = !dest.has_extension();

        if ((folder && !fs::exists(dest)) || (!folder && !fs::exists(dest.parent_path())))
        {
            fs::create_directories(folder ? dest : dest.parent_path());
        }

        auto output_path = folder ? dest / file.name : dest;
        std::ofstream output(output_path, std::ios::out | std::ios::binary);

        auto content = get_data(file);

        output.write(content.data(), static_cast<std::streamsize>(content.size()));
        output.close();

        logger::get()->debug("extracted '{}' to '{}'", file.name, output_path.string());
    }

    tl::expected<archive, archive_error> archive::from(const fs::path &path)
    {
        if (!fs::exists(path) || !fs::is_regular_file(path))
        {
            logger::get()->error("'{}' does not exist or is not a file", path.string());
            return tl::make_unexpected(archive_error::bad_file);
        }

        static constexpr std::uint8_t header[] = {'r', 'a', 'c', impl::version};
        static constexpr auto header_size = sizeof(header);

        std::ifstream file{path, std::ios::binary};
        file.unsetf(std::ios::skipws);

        char buf[header_size];
        file.read(buf, header_size);

        if (memcmp(buf, header, header_size) != 0)
        {
            logger::get()->error("'{}' is missing car header", path.string());
            return tl::make_unexpected(archive_error::no_header);
        }

        archive rtn;

        rtn.m_impl->location = path;
        rtn.m_impl->start = file.tellg();
        rtn.m_impl->file = std::move(file);

        return rtn;
    }
} // namespace solar2d
