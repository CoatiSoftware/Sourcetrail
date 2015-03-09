#include "utility/file/FilePath.h"

FilePath::FilePath(const char* filePath)
	: m_path(filePath)
{
}

FilePath::FilePath(const std::string& filePath)
	: m_path(filePath)
{
}

FilePath::FilePath(const boost::filesystem::path& filePath)
	: m_path(filePath)
{
}

bool FilePath::exists() const
{
	return boost::filesystem::exists(m_path);
}

std::string FilePath::str() const
{
	return m_path.generic_string();
}

std::string FilePath::absoluteStr() const
{
	return boost::filesystem::absolute(m_path).generic_string();
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
