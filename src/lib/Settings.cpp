#include "Settings.h"

#include "utility/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

Settings::Settings()
{
	clear();
}

Settings::~Settings()
{
}

bool Settings::load(const std::string& filePath)
{
	if (FileSystem::exists(filePath))
	{
		m_config = ConfigManager::createAndLoad(TextAccess::createFromFile(filePath));
		return true;
	}
	else
	{
		m_config = ConfigManager::createEmpty();
		LOG_WARNING("File for Settings not found.");
		return false;
	}
}

void Settings::save(const std::string& filePath)
{
	if (m_config)
	{
		m_config->save();
	}
	else
	{
		LOG_WARNING("Settings were not saved.");
	}
}

void Settings::clear()
{
	m_config = ConfigManager::createEmpty();
}

std::vector<std::string> Settings::getValues(const std::string& key, std::string defaultValue) const
{
	std::string value = getValue<std::string>(key, defaultValue);

	if (value.size())
	{
		std::deque<std::string> values = utility::split(value, '|');
		return std::vector<std::string>(values.begin(), values.end());
	}

	return std::vector<std::string>();
}
