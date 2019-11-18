#include "SourceGroupSettingsWithSourcePaths.h"

#include "ProjectSettings.h"
#include "utility.h"

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

bool SourceGroupSettingsWithSourcePaths::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithSourcePaths* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithSourcePaths*>(other);

	return (otherPtr && utility::isPermutation(m_sourcePaths, otherPtr->m_sourcePaths));
}

void SourceGroupSettingsWithSourcePaths::load(const ConfigManager* config, const std::string& key)
{
	setSourcePaths(
		config->getValuesOrDefaults(key + "/source_paths/source_path", std::vector<FilePath>()));
}

void SourceGroupSettingsWithSourcePaths::save(ConfigManager* config, const std::string& key)
{
	config->setValues(key + "/source_paths/source_path", getSourcePaths());
}
