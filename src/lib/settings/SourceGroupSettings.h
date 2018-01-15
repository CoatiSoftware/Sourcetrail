#ifndef SOURCE_GROUP_SETTINGS_H
#define SOURCE_GROUP_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/ProjectSettings.h"
#include "settings/SourceGroupStatusType.h"
#include "settings/SourceGroupType.h"

class ProjectSettings;

class SourceGroupSettings
{
public:
	static std::string s_keyPrefix;

	SourceGroupSettings(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettings();

	virtual void load(std::shared_ptr<const ConfigManager> config);
	virtual void save(std::shared_ptr<ConfigManager> config);

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	std::string getId() const;
	void setId(const std::string& id);

	std::string getName() const;
	void setName(const std::string& name);

	FilePath getProjectDirectoryPath() const;
	FilePath makePathExpandedAndAbsolute(const FilePath& path) const;
	std::vector<FilePath> makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const;

	virtual std::vector<std::string> getAvailableLanguageStandards() const = 0;
	virtual SourceGroupType getType() const;

	SourceGroupStatusType getStatus() const;
	void setStatus(SourceGroupStatusType status);

	std::string getStandard() const;
	void setStandard(const std::string& standard);

	std::vector<FilePath> getSourcePaths() const;
	std::vector<FilePath> getSourcePathsExpandedAndAbsolute() const;
	void setSourcePaths(const std::vector<FilePath>& sourcePaths);

	std::vector<FilePath> getExcludePaths() const;
	std::vector<FilePath> getExcludePathsExpandedAndAbsolute() const;
	void setExcludePaths(const std::vector<FilePath>& excludePaths);

	std::vector<std::string> getSourceExtensions() const;
	void setSourceExtensions(const std::vector<std::string>& sourceExtensions);

protected:
	template<typename T>
	static T getValue(const std::string& key, T defaultValue, std::shared_ptr<const ConfigManager> config);

	template<typename T>
	static std::vector<T> getValues(const std::string& key, std::vector<T> defaultValues, std::shared_ptr<const ConfigManager> config);

	static std::vector<FilePath> getPathValues(const std::string& key, std::shared_ptr<const ConfigManager> config);

	template<typename T>
	static bool setValue(const std::string& key, T value, std::shared_ptr<ConfigManager> config);

	template<typename T>
	static bool setValues(const std::string& key, std::vector<T> values, std::shared_ptr<ConfigManager> config);

	static bool setPathValues(const std::string& key, const std::vector<FilePath>& paths, std::shared_ptr<ConfigManager> config);

	const ProjectSettings* m_projectSettings;

private:
	virtual std::vector<std::string> getDefaultSourceExtensions() const = 0;
	virtual std::string getDefaultStandard() const = 0;

	std::string m_id;
	std::string m_name;
	const SourceGroupType m_type;
	SourceGroupStatusType m_status;

	std::string m_standard;
	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePath> m_excludePaths;
	std::vector<std::string> m_sourceExtensions;
};

template<typename T>
T SourceGroupSettings::getValue(const std::string& key, T defaultValue, std::shared_ptr<const ConfigManager> config)
{
	if (config)
	{
		T value;
		if (config->getValue(key, value))
		{
			return value;
		}
	}
	return defaultValue;
}

template<typename T>
std::vector<T> SourceGroupSettings::getValues(const std::string& key, std::vector<T> defaultValues, std::shared_ptr<const ConfigManager> config)
{
	if (config)
	{
		std::vector<T> values;
		if (config->getValues(key, values))
		{
			return values;
		}
	}
	return defaultValues;
}

template<typename T>
bool SourceGroupSettings::setValue(const std::string& key, T value, std::shared_ptr<ConfigManager> config)
{
	if (config)
	{
		config->setValue(key, value);
		return true;
	}
	return false;
}

template<typename T>
bool SourceGroupSettings::setValues(const std::string& key, std::vector<T> values, std::shared_ptr<ConfigManager> config)
{
	if (config)
	{
		config->setValues(key, values);
		return true;
	}
	return false;
}

#endif // SOURCE_GROUP_SETTINGS_H
