#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <memory>
#include <vector>

#include "utility/ConfigManager.h"
#include "utility/file/FilePath.h"

#include "settings/migration/SettingsMigrator.h"
#include "settings/migration/Migration.h"

class Settings
{
public:
	Settings(const Settings& other);
	Settings& operator=(const Settings& other);
	virtual ~Settings();

	bool load(const FilePath& filePath);
	void save();
	void save(const FilePath& filePath);

	void clear();

	virtual const FilePath& getFilePath() const;

	size_t getVersion() const;
	void setVersion(size_t version);

	static FilePath expandPath(const FilePath& path);
	static std::vector<FilePath> expandPaths(const std::vector<FilePath>& paths);

protected:
	Settings();

	void setFilePath(const FilePath& filePath);

	template<typename T>
	T getValue(const std::string& key, T defaultValue) const;

	template<typename T>
	std::vector<T> getValues(const std::string& key, std::vector<T> defaultValues) const;

	std::vector<FilePath> getPathValues(const std::string& key) const;

	template<typename T>
	bool setValue(const std::string& key, T value);

	template<typename T>
	bool setValues(const std::string& key, std::vector<T> values);

	bool setPathValues(const std::string& key, const std::vector<FilePath>& paths);

	bool isValueDefined(const std::string& key) const;

	void removeValues(const std::string& key);

	void enableWarnings() const;
	void disableWarnings() const;

	friend bool SettingsMigrator::migrate(Settings* settings, size_t targetVersion) const;

	std::shared_ptr<ConfigManager> m_config;

private:
	FilePath m_filePath;

	friend SettingsMigrator;
	friend Migration;
};

template<typename T>
T Settings::getValue(const std::string& key, T defaultValue) const
{
	if (m_config)
	{
		T value;
		if (m_config->getValue(key, value))
		{
			return value;
		}
	}
	return defaultValue;
}

template<typename T>
std::vector<T> Settings::getValues(const std::string& key, std::vector<T> defaultValues) const
{
	if(m_config)
	{
		std::vector<T> values;
		if(m_config->getValues(key, values))
		{
			return values;
		}
	}
	return defaultValues;
}

template<typename T>
bool Settings::setValue(const std::string& key, T value)
{
	if (m_config)
	{
		m_config->setValue(key, value);
		return true;
	}
	return false;
}

template<typename T>
bool Settings::setValues(const std::string& key, std::vector<T> values)
{
	if (m_config)
	{
		m_config->setValues(key, values);
		return true;
	}
	return false;
}

#endif // SETTINGS_H
