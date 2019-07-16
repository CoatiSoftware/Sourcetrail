#ifndef SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
#define SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H

#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "SourceGroupSettingsWithSourceExtensions.h"

class SourceGroupSettingsCxxCodeblocks
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithCppStandard
	, public SourceGroupSettingsWithCStandard
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithIndexedHeaderPaths
	, public SourceGroupSettingsWithSourceExtensions
{
public:
	SourceGroupSettingsCxxCodeblocks(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getCodeblocksProjectPath() const;
	FilePath getCodeblocksProjectPathExpandedAndAbsolute() const;
	void setCodeblocksProjectPath(const FilePath& compilationDatabasePath);

private:
	std::vector<std::wstring> getDefaultSourceExtensions() const override;

	FilePath m_codeblocksProjectPath;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_CODEBLOCKS_H
