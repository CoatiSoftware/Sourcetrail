#ifndef SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H
#define SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithSonargraphProjectPath
	: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithSonargraphProjectPath() = default;

	FilePath getSonargraphProjectPath() const;
	FilePath getSonargraphProjectPathExpandedAndAbsolute() const;
	void setSonargraphProjectPath(const FilePath& sonargraphProjectPath);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_sonargraphProjectPath;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_SONARGRAPH_PROJECT_PATH_H
