#include "settings/Settings.h"

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

Settings::Settings(const Settings& other)
	: m_filePath(other.m_filePath)
	, m_config(other.m_config->createCopy())
{
}

Settings& Settings::operator=(const Settings& other)
{
	if (&other != this)
	{
		m_filePath = other.m_filePath;
		m_config = other.m_config->createCopy();
	}

	return *this;
}

Settings::~Settings()
{
}

bool Settings::load(const FilePath& filePath)
{
	if (filePath.exists())
	{
		m_config = ConfigManager::createAndLoad(TextAccess::createFromFile(filePath.str()));
		m_filePath = filePath;
		return true;
	}
	else
	{
		clear();
		LOG_WARNING("File for Settings not found: " + filePath.str());
		return false;
	}
}

void Settings::save()
{
	if (m_config.get() && m_filePath.exists())
	{
		m_config->save(m_filePath.str());
	}
	else
	{
		LOG_WARNING("Settings were not saved: " + m_filePath.str());
	}
}

void Settings::save(const FilePath& filePath)
{
	setFilePath(filePath);

	if (m_config)
	{
		m_config->save(filePath.str());
	}
	else
	{
		LOG_WARNING("Settings were not saved: " + filePath.str());
	}
}

void Settings::clear()
{
	m_config = ConfigManager::createEmpty();
	m_filePath = FilePath();
}

const FilePath& Settings::getFilePath() const
{
	return m_filePath;
}

Settings::Settings()
{
	clear();
}

void Settings::setFilePath(const FilePath& filePath)
{
	m_filePath = filePath;
}

std::vector<FilePath> Settings::getPathValues(const std::string& key) const
{
	std::vector<std::string> values;
	values = getValues(key, values);

	std::vector<FilePath> paths;
	for (const std::string& path : values)
	{
		paths.push_back(FilePath(path));
	}
	return paths;
}

std::vector<FilePath> Settings::getRelativePathValues(const std::string& key) const
{
	std::vector<std::string> values;
	values = getValues(key, values);

	std::vector<FilePath> paths;
	for (const std::string& path : values)
	{
		FilePath filePath(path);
		if (!filePath.isAbsolute())
		{
			filePath = m_filePath.parentDirectory().concat(filePath);
		}

		paths.push_back(filePath.canonical());
	}
	return paths;
}

bool Settings::setPathValues(const std::string& key, const std::vector<FilePath>& paths)
{
	std::vector<std::string> values;
	for (const FilePath& path : paths)
	{
		values.push_back(path.str());
	}

	return setValues(key, values);
}

bool Settings::moveRelativePathValues(const std::string& key, const FilePath& filePath)
{
	std::vector<std::string> values;
	values = getValues(key, values);

	FilePath oldPath = m_filePath.absolute();
	FilePath newPath = filePath.absolute();

	for (size_t i = 0; i < values.size(); i++)
	{
		FilePath path(values[i]);
		if (!path.isAbsolute())
		{
			path = oldPath.parentDirectory().concat(path);
			path = path.canonical().relativeTo(newPath);
			values[i] = path.str();
		}
	}

	return setValues(key, values);
}

bool Settings::isValueDefined(const std::string& key) const
{
	return m_config->isValueDefined(key);
}

void Settings::enableWarnings() const
{
	m_config->setWarnOnEmptyKey(true);
}

void Settings::disableWarnings() const
{
	m_config->setWarnOnEmptyKey(false);
}
