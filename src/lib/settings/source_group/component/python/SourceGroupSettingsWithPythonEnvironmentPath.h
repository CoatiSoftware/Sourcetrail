#ifndef SOURCE_GROUP_SETTINGS_WITH_PYTHON_ENVIRONMENT_PATH_H
#define SOURCE_GROUP_SETTINGS_WITH_PYTHON_ENVIRONMENT_PATH_H

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithPythonEnvironmentPath: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithPythonEnvironmentPath() = default;

	FilePath getEnvironmentPath() const;
	FilePath getEnvironmentPathExpandedAndAbsolute() const;
	void setEnvironmentPath(const FilePath& environmentPath);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_environmentPath;
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_PYTHON_ENVIRONMENT_PATH_H
