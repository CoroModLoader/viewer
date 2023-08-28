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

            entry.type = static_cast<type>(m_impl->read());
            entry.offset = m_impl->read();
            entry.name = m_impl->read<std::string>();

            rtn.emplace_back(entry);
        }

        return rtn;
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
        auto folder = !dest.has_filename();

        if ((folder && !fs::exists(dest)) || (!folder && !fs::exists(dest.parent_path())))
        {
            fs::create_directories(folder ? dest : dest.parent_path());
        }

        auto output_path = folder ? dest / file.name : dest;
        std::ofstream output(output_path);

        auto buf = data(file);

        output.write(buf.data(), static_cast<std::streamsize>(buf.size()));
        output.close();

        logger::get()->debug("Extracted '{}' to '{}'", file.name, output_path.string());
    }

    std::optional<archive> archive::from(const fs::path &path)
    {
        if (!fs::exists(path) || !fs::is_regular_file(path))
        {
            logger::get()->error("'{}' does not exist or is not a file", path.string());
            return std::nullopt;
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
            return std::nullopt;
        }

        archive rtn;

        rtn.m_impl->start = file.tellg();
        rtn.m_impl->file = std::move(file);

        return rtn;
    }
} // namespace solar2d