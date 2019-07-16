#include "SourceGroupSettingsWithSourcePaths.h"

#include "ProjectSettings.h"
#include "utility.h"

SourceGroupSettingsWithSourcePaths::SourceGroupSettingsWithSourcePaths()
	: m_sourcePaths(std::vector<FilePath>())
{
}

bool SourceGroupSettingsWithSourcePaths::equals(std::shared_ptr<SourceGroupSettingsWithSourcePaths> other) const
{
	return utility::isPermutation(m_sourcePaths, other->m_sourcePaths);
}

void SourceGroupSettingsWithSourcePaths::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setSourcePaths(config->getValuesOrDefaults(key + "/source_paths/source_path", std::vector<FilePath>()));
}

void SourceGroupSettingsWithSourcePaths::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/source_paths/source_path", getSourcePaths());
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
