#include "SourceGroupSettingsWithCxxCodeblocksPath.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "ProjectSettings.h"
#include "utilityFile.h"

FilePath SourceGroupSettingsWithCxxCodeblocksPath::getCodeblocksProjectPath() const
{
	return m_codeblocksProjectPath;
}

FilePath SourceGroupSettingsWithCxxCodeblocksPath::getCodeblocksProjectPathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(getCodeblocksProjectPath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithCxxCodeblocksPath::setCodeblocksProjectPath(const FilePath& codeblocksProjectPath)
{
	m_codeblocksProjectPath = codeblocksProjectPath;
}

bool SourceGroupSettingsWithCxxCodeblocksPath::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCxxCodeblocksPath* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCxxCodeblocksPath*>(other);

	return (
		otherPtr &&
		m_codeblocksProjectPath == otherPtr->m_codeblocksProjectPath
	);
}

void SourceGroupSettingsWithCxxCodeblocksPath::load(const ConfigManager* config, const std::string& key)
{
	setCodeblocksProjectPath(config->getValueOrDefault(key + "/codeblocks_project_path", FilePath(L"")));
}

void SourceGroupSettingsWithCxxCodeblocksPath::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/codeblocks_project_path", getCodeblocksProjectPath().wstr());
}

#endif // BUILD_CXX_LANGUAGE_PACKAGE
