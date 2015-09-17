#include "utility/file/FilePath.h"

FilePath::FilePath()
	: m_exists(false)
{
}

FilePath::FilePath(const char* filePath)
	: m_path(filePath)
	, m_exists(false)
{
	init();
}

FilePath::FilePath(const std::string& filePath)
	: m_path(filePath)
	, m_exists(false)
{
	init();
}

FilePath::FilePath(const boost::filesystem::path& filePath)
	: m_path(filePath)
	, m_exists(false)
{
	init();
}

boost::filesystem::path FilePath::path() const
{
	return m_path;
}

bool FilePath::empty() const
{
	return m_path.empty();
}

bool FilePath::exists() const
{
	return m_exists;
}

bool FilePath::isDirectory() const
{
	return boost::filesystem::is_directory(m_path);
}

bool FilePath::isAbsolute() const
{
	return m_path.is_absolute();
}

FilePath FilePath::parentDirectory() const
{
	return m_path.parent_path();
}

FilePath FilePath::absolute() const
{
	return boost::filesystem::absolute(m_path);
}

FilePath FilePath::canonical() const
{
	if (m_exists)
	{
		return boost::filesystem::canonical(m_path);
	}

	return FilePath(m_path);
}

FilePath FilePath::relativeTo(const FilePath& other) const
{
	boost::filesystem::path a = m_path;
	boost::filesystem::path b = other.m_path;

	if (a.root_path() != b.root_path())
	{
		return str();
	}

	boost::filesystem::path::const_iterator itA = a.begin();
	boost::filesystem::path::const_iterator itB = b.begin();

	while (*itA == *itB && itA != a.end() && itB != b.end())
	{
		itA++;
		itB++;
	}

	boost::filesystem::path r;

	if (itB != b.end())
	{
		itB++;

		for (; itB != b.end(); itB++)
		{
			r /= "..";
		}
	}

	for (; itA != a.end(); itA++)
	{
		r /= *itA;
	}

	return r;
}

FilePath FilePath::concat(const FilePath& other) const
{
	return boost::filesystem::path(m_path) / other.m_path;
}

std::string FilePath::str() const
{
	return m_path.generic_string();
}

std::string FilePath::fileName() const
{
	return m_path.filename().generic_string();
}

std::string FilePath::extension() const
{
	return m_path.extension().generic_string();
}

FilePath FilePath::withoutExtension() const
{
	return FilePath(boost::filesystem::path(m_path).replace_extension());
}

FilePath FilePath::replaceExtension(const std::string& extension)
{
	return FilePath(boost::filesystem::path(m_path).replace_extension(extension));
}

bool FilePath::hasExtension(const std::vector<std::string>& extensions) const
{
	std::string e = extension();
	for (std::string ext : extensions)
	{
		if (e == ext)
		{
			return true;
		}
	}
	return false;
}

bool FilePath::operator==(const FilePath& other) const
{
	if (exists() && other.exists())
	{
		return boost::filesystem::equivalent(m_path, other.m_path);
	}

	return m_path.compare(other.m_path) == 0;
}

bool FilePath::operator!=(const FilePath& other) const
{
	return !(*this == other);
}

bool FilePath::operator<(const FilePath& other) const
{
	return m_path.compare(other.m_path) < 0;
}

void FilePath::init()
{
	if (boost::filesystem::exists(m_path))
	{
		m_exists = true;
	}
}
