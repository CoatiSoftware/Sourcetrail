#include "SourceGroupSettingsWithCxxPchOptions.h"

#include "ProjectSettings.h"
#include "utilityFile.h"

bool SourceGroupSettingsWithCxxPchOptions::equals(std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> other) const
{
	return (
		other &&
		m_pchInputFilePath == other->m_pchInputFilePath
	);
}

void SourceGroupSettingsWithCxxPchOptions::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setPchInputFilePathFilePath(config->getValueOrDefault(key + "/pch_input_file_path", FilePath(L"")));
}

void SourceGroupSettingsWithCxxPchOptions::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/pch_input_file_path", getPchInputFilePath().wstr());
}

FilePath SourceGroupSettingsWithCxxPchOptions::getPchDependenciesDirectoryPath() const
{
	return getSourceGroupDependenciesDirectoryPath().concatenate(L"pch");
}

FilePath SourceGroupSettingsWithCxxPchOptions::getPchInputFilePath() const
{
	return m_pchInputFilePath;
}

FilePath SourceGroupSettingsWithCxxPchOptions::getPchInputFilePathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(getPchInputFilePath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithCxxPchOptions::setPchInputFilePathFilePath(const FilePath& path)
{
	m_pchInputFilePath = path;
}
