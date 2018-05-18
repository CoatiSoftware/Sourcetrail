#include "settings/SourceGroupSettingsWithSourcePaths.h"

#include "settings/ProjectSettings.h"
#include "utility/utility.h"

SourceGroupSettingsWithSourcePaths::SourceGroupSettingsWithSourcePaths()
	: m_sourcePaths(std::vector<FilePath>())
	, m_sourceExtensions(std::vector<std::wstring>())
{
}

bool SourceGroupSettingsWithSourcePaths::equals(std::shared_ptr<SourceGroupSettingsWithSourcePaths> other) const
{
	return (
		utility::isPermutation(m_sourcePaths, other->m_sourcePaths) &&
		utility::isPermutation(m_sourceExtensions, other->m_sourceExtensions)
	);
}

void SourceGroupSettingsWithSourcePaths::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setSourcePaths(config->getValuesOrDefaults(key + "/source_paths/source_path", std::vector<FilePath>()));
	setSourceExtensions(config->getValuesOrDefaults(key + "/source_extensions/source_extension", std::vector<std::wstring>()));
}

void SourceGroupSettingsWithSourcePaths::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/source_paths/source_path", getSourcePaths());
	config->setValues(key + "/source_extensions/source_extension", getSourceExtensions());
}

std::vector<FilePath> SourceGroupSettingsWithSourcePaths::getSourcePaths() const
{
	return m_sourcePaths;
}

std::vector<FilePath> SourceGroupSettingsWithSourcePaths::getSourcePathsExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getSourcePaths());
}

void SourceGroupSettingsWithSourcePaths::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	m_sourcePaths = sourcePaths;
}

std::vector<std::wstring> SourceGroupSettingsWithSourcePaths::getSourceExtensions() const
{
	if (m_sourceExtensions.empty())
	{
		return getDefaultSourceExtensions();
	}
	return m_sourceExtensions;
}

void SourceGroupSettingsWithSourcePaths::setSourceExtensions(const std::vector<std::wstring>& sourceExtensions)
{
	m_sourceExtensions = sourceExtensions;
}
