#include "utility/file/FilePath.h"

#include <regex>

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

FilePath::FilePath()
	: m_exists(false)
	, m_checkedExists(false)
{
}

FilePath::FilePath(const char* filePath)
	: m_path(filePath)
	, m_exists(false)
	, m_checkedExists(false)
{
}

FilePath::FilePath(const std::string& filePath)
	: m_path(filePath)
	, m_exists(false)
	, m_checkedExists(false)
{
}

FilePath::FilePath(const boost::filesystem::path& filePath)
	: m_path(filePath)
	, m_exists(false)
	, m_checkedExists(false)
{
}

FilePath::FilePath(const std::string& filePath, const std::string& base)
	: m_path(boost::filesystem::absolute(filePath, base))
	, m_exists(false)
	, m_checkedExists(false)
{
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
	if (!m_checkedExists)
	{
		m_exists = boost::filesystem::exists(m_path);
		m_checkedExists = true;
	}

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
	if (!exists())
	{
		return FilePath(m_path);
	}

	boost::filesystem::path abs_p = boost::filesystem::absolute(m_path);
	boost::filesystem::path result;
	for (boost::filesystem::path::iterator it = abs_p.begin(); it != abs_p.end(); ++it)
	{
		if (*it == "..")
		{
			// /a/b/.. is not necessarily /a if b is a symbolic link
			if (boost::filesystem::is_symlink(result))
				result /= *it;
			// /a/b/../.. is not /a/b/.. under most circumstances
			// We can end up with ..s in our result because of symbolic links
			else if (result.filename() == "..")
				result /= *it;
			// Otherwise it should be safe to resolve the parent
			else
				result = result.parent_path();
		}
		else if (*it == ".")
		{
			// Ignore
		}
		else
		{
			// Just cat other path entries
			result /= *it;
		}
	}
	return result;
}

std::vector<FilePath> FilePath::expandEnvironmentVariables() const
{
	std::vector<FilePath> paths;
	std::string text = str();

	static std::regex env("\\$\\{([^}]+)\\}|%([^%]+)%"); // ${VARIABLE_NAME} or %VARIABLE_NAME%
	std::smatch match;
	while (std::regex_search(text, match, env))
	{
		const char * s = match[1].matched ? getenv(match[1].str().c_str()) : getenv(match[2].str().c_str());
		if (s == nullptr)
		{
			LOG_ERROR(match[1].str() + " is not an environment variable");
			return paths;
		}
		text.replace( match.position(0), match.length(0), s);
	}

	char environmentVariablePathSeparator = ':';

#if defined(_WIN32) || defined(_WIN64)
	environmentVariablePathSeparator = ';';
#endif

	for (const std::string& str : utility::splitToVector(text, environmentVariablePathSeparator))
	{
		if (str.size())
		{
			paths.push_back(str);
		}
	}

	return paths;
}

FilePath FilePath::relativeTo(const FilePath& other) const
{
	boost::filesystem::path a = this->canonical().m_path;
	boost::filesystem::path b = other.canonical().m_path;

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
		if (!boost::filesystem::is_directory(b))
		{
			itB++;
		}

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

bool FilePath::contains(const FilePath& other) const
{
	if (!isDirectory())
	{
		return false;
	}

	boost::filesystem::path dir = m_path;
	const boost::filesystem::path& dir2 = other.m_path;

	if (dir.filename() == ".")
	{
		dir.remove_filename();
	}

	auto it = dir.begin();
	auto it2 = dir2.begin();

	while (it != dir.end())
	{
		if (it2 == dir2.end())
		{
			return false;
		}

		if (*it != *it2)
		{
			return false;
		}

		it++;
		it2++;
	}

	return true;
}

std::string FilePath::str() const
{
	return m_path.generic_string();
}

std::string FilePath::getBackslashedString() const
{
	return utility::replace(str(), "/", "\\");
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

FilePath FilePath::replaceExtension(const std::string& extension) const
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
