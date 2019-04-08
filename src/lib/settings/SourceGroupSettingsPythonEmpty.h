#ifndef SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
#define SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H

#include "FilePath.h"
#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithSourceExtensions.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsPythonEmpty
	: public SourceGroupSettings
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithSourceExtensions
	, public SourceGroupSettingsWithSourcePaths
{
public:
	SourceGroupSettingsPythonEmpty(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getEnvironmentDirectoryPath() const;
	FilePath getEnvironmentDirectoryPathExpandedAndAbsolute() const;
	void setEnvironmentDirectoryPath(const FilePath& environmentDirectoryPath);

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;

	FilePath m_environmentDirectoryPath;
};

#endif // SOURCE_GROUP_SETTINGS_PYTHON_EMPTY_H
