#include "SourceGroupSettingsCxxCodeblocks.h"

#include "ProjectSettings.h"
#include "ConfigManager.h"

SourceGroupSettingsCxxCodeblocks::SourceGroupSettingsCxxCodeblocks(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_CXX_CODEBLOCKS, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsCxxCodeblocks::createCopy() const
{
	return std::make_shared<SourceGroupSettingsCxxCodeblocks>(*this);
}

void SourceGroupSettingsCxxCodeblocks::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::load(config, key);
	SourceGroupSettingsWithCStandard::load(config, key);
	SourceGroupSettingsWithExcludeFilters::load(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::load(config, key);
	SourceGroupSettingsWithSourceExtensions::load(config, key);

	setCodeblocksProjectPath(config->getValueOrDefault(key + "/codeblocks_project_path", FilePath(L"")));
}

void SourceGroupSettingsCxxCodeblocks::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithCppStandard::save(config, key);
	SourceGroupSettingsWithCStandard::save(config, key);
	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::save(config, key);
	SourceGroupSettingsWithSourceExtensions::save(config, key);

	config->setValue(key + "/codeblocks_project_path", getCodeblocksProjectPath().wstr());
}

bool SourceGroupSettingsCxxCodeblocks::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> otherCxxCodeblocks = std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(other);

	return (
		otherCxxCodeblocks &&
		SourceGroupSettingsCxx::equals(other) &&
		SourceGroupSettingsWithCppStandard::equals(otherCxxCodeblocks) &&
		SourceGroupSettingsWithCStandard::equals(otherCxxCodeblocks) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherCxxCodeblocks) &&
		SourceGroupSettingsWithIndexedHeaderPaths::equals(otherCxxCodeblocks) &&
		SourceGroupSettingsWithSourceExtensions::equals(otherCxxCodeblocks) &&
		m_codeblocksProjectPath == otherCxxCodeblocks->m_codeblocksProjectPath
	);
}

FilePath SourceGroupSettingsCxxCodeblocks::getCodeblocksProjectPath() const
{
	return m_codeblocksProjectPath;
}

FilePath SourceGroupSettingsCxxCodeblocks::getCodeblocksProjectPathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getCodeblocksProjectPath());
}

void SourceGroupSettingsCxxCodeblocks::setCodeblocksProjectPath(const FilePath& compilationDatabasePath)
{
	m_codeblocksProjectPath = compilationDatabasePath;
}

std::vector<std::wstring> SourceGroupSettingsCxxCodeblocks::getDefaultSourceExtensions() const
{
	return {
		L".c",
		L".cpp",
		L".cxx",
		L".cc"
	};
}
