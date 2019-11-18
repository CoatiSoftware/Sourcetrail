#ifndef SOURCE_GROUP_SETTINGS_WITH_CXX_CODEBLOCKS_PATH_H
#define SOURCE_GROUP_SETTINGS_WITH_CXX_CODEBLOCKS_PATH_H

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithCxxCodeblocksPath: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithCxxCodeblocksPath() = default;

	FilePath getCodeblocksProjectPath() const;
	FilePath getCodeblocksProjectPathExpandedAndAbsolute() const;
	void setCodeblocksProjectPath(const FilePath& codeblocksProjectPath);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_codeblocksProjectPath;
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_CXX_CODEBLOCKS_PATH_H
