#include "settings/SourceGroupSettingsCxx.h"

#include "settings/ProjectSettings.h"
#include "utility/ConfigManager.h"
#include "utility/utility.h"

SourceGroupSettingsCxx::SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_headerSearchPaths(std::vector<FilePath>())
	, m_frameworkSearchPaths(std::vector<FilePath>())
	, m_compilerFlags(std::vector<std::wstring>())
{
}

void SourceGroupSettingsCxx::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	setHeaderSearchPaths(config->getValuesOrDefaults(key + "/header_search_paths/header_search_path", std::vector<FilePath>()));
	setFrameworkSearchPaths(config->getValuesOrDefaults(key + "/framework_search_paths/framework_search_path", std::vector<FilePath>()));
	setCompilerFlags(config->getValuesOrDefaults(key + "/compiler_flags/compiler_flag", std::vector<std::wstring>()));
}

void SourceGroupSettingsCxx::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	config->setValues(key + "/header_search_paths/header_search_path", getHeaderSearchPaths());
	config->setValues(key + "/framework_search_paths/framework_search_path", getFrameworkSearchPaths());
	config->setValues(key + "/compiler_flags/compiler_flag", getCompilerFlags());
}

bool SourceGroupSettingsCxx::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxx> otherCxx = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(other);

	return (
		otherCxx &&
		SourceGroupSettings::equals(other) &&
		utility::isPermutation(m_headerSearchPaths, otherCxx->m_headerSearchPaths) &&
		utility::isPermutation(m_frameworkSearchPaths, otherCxx->m_frameworkSearchPaths) &&
		utility::isPermutation(m_compilerFlags, otherCxx->m_compilerFlags)
	);
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPaths() const
{
	return m_headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getHeaderSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getHeaderSearchPaths());
}

void SourceGroupSettingsCxx::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	m_headerSearchPaths = headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsCxx::getFrameworkSearchPathsExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getFrameworkSearchPaths());
}

void SourceGroupSettingsCxx::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	m_frameworkSearchPaths = frameworkSearchPaths;
}

std::vector<std::wstring> SourceGroupSettingsCxx::getCompilerFlags() const
{
	return m_compilerFlags;
}

void SourceGroupSettingsCxx::setCompilerFlags(const std::vector<std::wstring>& compilerFlags)
{
	m_compilerFlags = compilerFlags;
}
