#include "FilePath.h"

#include <regex>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include "logging.h"
#include "utilityString.h"

FilePath::FilePath()
	: m_path(std::make_unique<boost::filesystem::path>(""))
	, m_exists(false)
	, m_checkedExists(false)
	, m_isDirectory(false)
	, m_checkedIsDirectory(false)
	, m_canonicalized(false)
{
}

FilePath::FilePath(const std::string& filePath)
	: m_path(std::make_unique<boost::filesystem::path>(filePath))
	, m_exists(false)
	, m_checkedExists(false)
	, m_isDirectory(false)
	, m_checkedIsDirectory(false)
	, m_canonicalized(false)
{
}

FilePath::FilePath(const std::wstring& filePath)
	: m_path(std::make_unique<boost::filesystem::path>(filePath))
	, m_exists(false)
	, m_checkedExists(false)
	, m_isDirectory(false)
	, m_checkedIsDirectory(false)
	, m_canonicalized(false)
{
}

FilePath::FilePath(const FilePath& other)
	: m_path(std::make_unique<boost::filesystem::path>(other.getPath()))
	, m_exists(other.m_exists)
	, m_checkedExists(other.m_checkedExists)
	, m_isDirectory(other.m_isDirectory)
	, m_checkedIsDirectory(other.m_checkedIsDirectory)
	, m_canonicalized(other.m_canonicalized)
{
}

FilePath::FilePath(FilePath&& other)
	: m_path(std::move(other.m_path))
	, m_exists(other.m_exists)
	, m_checkedExists(other.m_checkedExists)
	, m_isDirectory(other.m_isDirectory)
	, m_checkedIsDirectory(other.m_checkedIsDirectory)
	, m_canonicalized(other.m_canonicalized)
{
}

FilePath::FilePath(const std::wstring& filePath, const std::wstring& base)
	: m_path(std::make_unique<boost::filesystem::path>(boost::filesystem::absolute(filePath, base)))
	, m_exists(false)
	, m_checkedExists(false)
	, m_isDirectory(false)
	, m_checkedIsDirectory(false)
	, m_canonicalized(false)
{
}

FilePath::~FilePath() {}

boost::filesystem::path FilePath::getPath() const
{
	return *(m_path.get());
}

bool FilePath::empty() const
{
	return m_path->empty();
}

bool FilePath::exists() const
{
	if (!m_checkedExists)
	{
		m_exists = boost::filesystem::exists(getPath());
		m_checkedExists = true;
	}

	return m_exists;
}

bool FilePath::recheckExists() const
{
	m_checkedExists = false;
	return exists();
}

bool FilePath::isDirectory() const
{
	if (!m_checkedIsDirectory)
	{
		m_isDirectory = boost::filesystem::is_directory(getPath());
		m_checkedIsDirectory = true;
	}

	return m_isDirectory;
}

bool FilePath::isAbsolute() const
{
	return m_path->is_absolute();
}

bool FilePath::isValid() const
{
	if (!isDirectory())
	{
		if (!boost::filesystem::portable_file_name(m_path->filename().generic_string()))
		{
			return false;
		}
	}

	boost::filesystem::path::iterator it = m_path->begin();

#if WIN32
	if (isAbsolute() && m_path->has_root_path())
	{
		std::string root = m_path->root_path().string();
		std::string current = "";
		while (current.size() < root.size())
		{
			current += it->string();
			it++;
		}
	}
#endif

	for (; it != m_path->end(); ++it)
	{
		std::string ss = it->string();
		if (!boost::filesystem::portable_directory_name(ss))
		{
			return false;
		}
	}

	return true;
}

FilePath FilePath::getParentDirectory() const
{
	FilePath parentDirectory(m_path->parent_path().wstring());

	if (!parentDirectory.empty())
	{
		parentDirectory.m_checkedIsDirectory = true;
		parentDirectory.m_isDirectory = true;

		if (m_checkedExists && m_exists)
		{
			parentDirectory.m_checkedExists = true;
			parentDirectory.m_exists = true;
		}
	}

	return parentDirectory;
}

FilePath& FilePath::makeAbsolute()
{
	m_path = std::make_unique<boost::filesystem::path>(boost::filesystem::absolute(getPath()));
	return *this;
}

FilePath FilePath::getAbsolute() const
{
	FilePath path(*this);
	path.makeAbsolute();
	return path;
}

FilePath& FilePath::makeCanonical()
{
	if (m_canonicalized || !exists())
	{
		return *this;
	}

	boost::filesystem::path canonicalPath;

#if defined(_WIN32)
	boost::filesystem::path abs_p = boost::filesystem::absolute(getPath());

	boost::filesystem::path::iterator it = abs_p.begin();

	// add first element before loop because this won't be recognized as absolute path yet
	canonicalPath /= *it;
	it++;

	for (; it != abs_p.end(); ++it)
	{
		if (*it == "..")
		{
			canonicalPath = canonicalPath.parent_path();
		}
		else if (*it == ".")
		{
			continue;
		}
		else
		{
			canonicalPath /= *it;

			if (boost::filesystem::is_symlink(boost::filesystem::symlink_status(canonicalPath)))
			{
				boost::filesystem::path symlink = boost::filesystem::read_symlink(canonicalPath);
				if (!symlink.empty())
				{
					// on Windows the read_symlink function discards the drive letter (this is a
					// boost bug). Therefore we need to make the path absolute again. We also have
					// to discard the trailing \0 characters so that we can continue appending to
					// the path.
					canonicalPath = utility::substrBeforeFirst(
						boost::filesystem::absolute(symlink).string(), '\0');
				}
			}
		}
	}
#else
	try
	{
		canonicalPath = boost::filesystem::canonical(getPath());
	}
	catch (boost::filesystem::filesystem_error e)
	{
		LOG_ERROR_STREAM(<< e.what());
		return *this;
	}
#endif
	m_path = std::make_unique<boost::filesystem::path>(canonicalPath);
	m_canonicalized = true;
	return *this;
}

FilePath FilePath::getCanonical() const
{
	FilePath path(*this);
	path.makeCanonical();
	return path;
}

std::vector<FilePath> FilePath::expandEnvironmentVariables() const
{
	std::vector<FilePath> paths;
	std::string text = str();

	static std::regex env("\\$\\{([^}]+)\\}|%([^%]+)%");	// ${VARIABLE_NAME} or %VARIABLE_NAME%
	std::smatch match;
	while (std::regex_search(text, match, env))
	{
		const char* s = match[1].matched ? getenv(match[1].str().c_str())
										 : getenv(match[2].str().c_str());
		if (s == nullptr)
		{
			LOG_ERROR_STREAM(<< match[1].str() << " is not an environment variable in: " << text);
			return paths;
		}
		text.replace(match.position(0), match.length(0), s);
	}

	char environmentVariablePathSeparator = ':';

#if defined(_WIN32) || defined(_WIN64)
	environmentVariablePathSeparator = ';';
#endif

	for (const std::string& str: utility::splitToVector(text, environmentVariablePathSeparator))
	{
		if (str.size())
		{
			paths.push_back(FilePath(str));
		}
	}

	return paths;
}

FilePath& FilePath::makeRelativeTo(const FilePath& other)
{
	const boost::filesystem::path a = this->getCanonical().getPath();
	const boost::filesystem::path b = other.getCanonical().getPath();

	if (a.root_path() != b.root_path())
	{
		return *this;
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

	if (r.empty())
	{
		r = "./";
	}

	m_path = std::make_unique<boost::filesystem::path>(r);
	return *this;
}


FilePath FilePath::getRelativeTo(const FilePath& other) const
{
	FilePath path(*this);
	path.makeRelativeTo(other);
	return path;
}

FilePath& FilePath::concatenate(const FilePath& other)
{
	m_path->operator/=(other.getPath());
	m_exists = false;
	m_checkedExists = false;
	m_isDirectory = false;
	m_checkedIsDirectory = false;
	m_canonicalized = false;

	return *this;
}

FilePath FilePath::getConcatenated(const FilePath& other) const
{
	FilePath path(*this);
	path.concatenate(other);
	return path;
}

FilePath& FilePath::concatenate(const std::wstring& other)
{
	m_path->operator/=(other);
	m_exists = false;
	m_checkedExists = false;
	m_isDirectory = false;
	m_checkedIsDirectory = false;
	m_canonicalized = false;

	return *this;
}

FilePath FilePath::getConcatenated(const std::wstring& other) const
{
	FilePath path(*this);
	path.concatenate(other);
	return path;
}

FilePath FilePath::getLowerCase() const
{
	return FilePath(utility::toLowerCase(wstr()));
}

bool FilePath::contains(const FilePath& other) const
{
	if (!isDirectory())
	{
		return false;
	}

	boost::filesystem::path dir = getPath();
	const std::unique_ptr<boost::filesystem::path>& dir2 = other.m_path;

	if (dir.filename() == ".")
	{
		dir.remove_filename();
	}

	auto it = dir.begin();
	auto it2 = dir2->begin();

	while (it != dir.end())
	{
		if (it2 == dir2->end())
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
	return m_path->generic_string();
}

std::wstring FilePath::wstr() const
{
	return m_path->generic_wstring();
}

std::string FilePath::getBackslashedString() const
{
	return utility::replace(str(), "/", "\\");
}

std::wstring FilePath::getBackslashedWString() const
{
	return utility::replace(wstr(), L"/", L"\\");
}

std::wstring FilePath::fileName() const
{
	return m_path->filename().generic_wstring();
}

std::wstring FilePath::extension() const
{
	return m_path->extension().generic_wstring();
}

FilePath FilePath::withoutExtension() const
{
	return FilePath(getPath().replace_extension().wstring());
}

FilePath FilePath::replaceExtension(const std::wstring& extension) const
{
	return FilePath(getPath().replace_extension(extension).wstring());
}

bool FilePath::hasExtension(const std::vector<std::wstring>& extensions) const
{
	const std::wstring e = extension();
	for (const std::wstring& ext: extensions)
	{
		if (e == ext)
		{
			return true;
		}
	}
	return false;
}

FilePath& FilePath::operator=(const FilePath& other)
{
	m_path = std::make_unique<boost::filesystem::path>(other.getPath());
	m_exists = other.m_exists;
	m_checkedExists = other.m_checkedExists;
	m_isDirectory = other.m_isDirectory;
	m_checkedIsDirectory = other.m_checkedIsDirectory;
	m_canonicalized = other.m_canonicalized;
	return *this;
}

FilePath& FilePath::operator=(FilePath&& other)
{
	m_path = std::move(other.m_path);
	m_exists = other.m_exists;
	m_checkedExists = other.m_checkedExists;
	m_isDirectory = other.m_isDirectory;
	m_checkedIsDirectory = other.m_checkedIsDirectory;
	m_canonicalized = other.m_canonicalized;
	return *this;
}

bool FilePath::operator==(const FilePath& other) const
{
	if (exists() && other.exists())
	{
		return boost::filesystem::equivalent(getPath(), other.getPath());
	}

	return m_path->compare(other.getPath()) == 0;
}

bool FilePath::operator!=(const FilePath& other) const
{
	return !(*this == other);
}

bool FilePath::operator<(const FilePath& other) const
{
	return m_path->compare(other.getPath()) < 0;
}
