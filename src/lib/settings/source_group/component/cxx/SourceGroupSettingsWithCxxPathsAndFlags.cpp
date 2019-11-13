#include "SourceGroupSettingsWithCxxPathsAndFlags.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "ProjectSettings.h"
#include "utility.h"

std::vector<FilePath> SourceGroupSettingsWithCxxPathsAndFlags::getHeaderSearchPaths() const
{
	return m_headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsWithCxxPathsAndFlags::getHeaderSearchPathsExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getHeaderSearchPaths());
}

void SourceGroupSettingsWithCxxPathsAndFlags::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	m_headerSearchPaths = headerSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsWithCxxPathsAndFlags::getFrameworkSearchPaths() const
{
	return m_frameworkSearchPaths;
}

std::vector<FilePath> SourceGroupSettingsWithCxxPathsAndFlags::getFrameworkSearchPathsExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getFrameworkSearchPaths());
}

void SourceGroupSettingsWithCxxPathsAndFlags::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	m_frameworkSearchPaths = frameworkSearchPaths;
}

std::vector<std::wstring> SourceGroupSettingsWithCxxPathsAndFlags::getCompilerFlags() const
{
	return m_compilerFlags;
}

void SourceGroupSettingsWithCxxPathsAndFlags::setCompilerFlags(const std::vector<std::wstring>& compilerFlags)
{
	m_compilerFlags = compilerFlags;
}

bool SourceGroupSettingsWithCxxPathsAndFlags::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCxxPathsAndFlags* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCxxPathsAndFlags*>(other);

	return (
		otherPtr &&
		utility::isPermutation(m_headerSearchPaths, otherPtr->m_headerSearchPaths) &&
		utility::isPermutation(m_frameworkSearchPaths, otherPtr->m_frameworkSearchPaths) &&
		utility::isPermutation(m_compilerFlags, otherPtr->m_compilerFlags)
	);
}

void SourceGroupSettingsWithCxxPathsAndFlags::load(const ConfigManager* config, const std::string& key)
{
	setHeaderSearchPaths(config->getValuesOrDefaults(key + "/header_search_paths/header_search_path", std::vector<FilePath>()));
	setFrameworkSearchPaths(config->getValuesOrDefaults(key + "/framework_search_paths/framework_search_path", std::vector<FilePath>()));
	setCompilerFlags(config->getValuesOrDefaults(key + "/compiler_flags/compiler_flag", std::vector<std::wstring>()));
}

void SourceGroupSettingsWithCxxPathsAndFlags::save(ConfigManager* config, const std::string& key)
{
	config->setValues(key + "/header_search_paths/header_search_path", getHeaderSearchPaths());
	config->setValues(key + "/framework_search_paths/framework_search_path", getFrameworkSearchPaths());
	config->setValues(key + "/compiler_flags/compiler_flag", getCompilerFlags());
}

#endif // BUILD_CXX_LANGUAGE_PACKAGE
