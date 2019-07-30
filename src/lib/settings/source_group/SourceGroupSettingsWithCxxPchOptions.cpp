#include "SourceGroupSettingsWithCxxPchOptions.h"

#include "ProjectSettings.h"
#include "utility.h"
#include "utilityFile.h"

bool SourceGroupSettingsWithCxxPchOptions::equals(std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> other) const
{
	return (
		other &&
		m_pchInputFilePath == other->m_pchInputFilePath &&
		utility::isPermutation(m_pchFlags, other->m_pchFlags) &&
		m_useCompilerFlags == other->m_useCompilerFlags
	);
}

void SourceGroupSettingsWithCxxPchOptions::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setPchInputFilePathFilePath(config->getValueOrDefault(key + "/pch_input_file_path", FilePath(L"")));
	setPchFlags(config->getValuesOrDefaults(key + "/pch_flags/pch_flag", std::vector<std::wstring>()));
	setUseCompilerFlags(config->getValueOrDefault(key + "/pch_flags/use_compiler_flags", false));
}

void SourceGroupSettingsWithCxxPchOptions::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/pch_input_file_path", getPchInputFilePath().wstr());
	config->setValues(key + "/pch_flags/pch_flag", getPchFlags());
	config->setValue(key + "/pch_flags/use_compiler_flags", getUseCompilerFlags());
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

bool SourceGroupSettingsWithCxxPchOptions::getUseCompilerFlags() const
{
	return m_useCompilerFlags;
}

void SourceGroupSettingsWithCxxPchOptions::setUseCompilerFlags(bool useCompilerFlags)
{
	m_useCompilerFlags = useCompilerFlags;
}

std::vector<std::wstring> SourceGroupSettingsWithCxxPchOptions::getPchFlags() const
{
	return m_pchFlags;
}

void SourceGroupSettingsWithCxxPchOptions::setPchFlags(const std::vector<std::wstring>& pchFlags)
{
	m_pchFlags = pchFlags;
}
