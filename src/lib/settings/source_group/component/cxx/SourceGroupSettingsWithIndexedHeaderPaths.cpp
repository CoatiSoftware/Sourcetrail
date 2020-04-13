#include "SourceGroupSettingsWithIndexedHeaderPaths.h"

#include "../../../ProjectSettings.h"
#include "../../../../utility/utility.h"

std::vector<FilePath> SourceGroupSettingsWithIndexedHeaderPaths::getIndexedHeaderPaths() const
{
	return m_indexedHeaderPaths;
}

std::vector<FilePath> SourceGroupSettingsWithIndexedHeaderPaths::getIndexedHeaderPathsExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getIndexedHeaderPaths());
}

void SourceGroupSettingsWithIndexedHeaderPaths::setIndexedHeaderPaths(
	const std::vector<FilePath>& indexedHeaderPaths)
{
	m_indexedHeaderPaths = indexedHeaderPaths;
}

bool SourceGroupSettingsWithIndexedHeaderPaths::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithIndexedHeaderPaths* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithIndexedHeaderPaths*>(other);

	return (otherPtr && utility::isPermutation(m_indexedHeaderPaths, otherPtr->m_indexedHeaderPaths));
}

void SourceGroupSettingsWithIndexedHeaderPaths::load(const ConfigManager* config, const std::string& key)
{
	setIndexedHeaderPaths(config->getValuesOrDefaults(
		key + "/indexed_header_paths/indexed_header_path", std::vector<FilePath>()));
}

void SourceGroupSettingsWithIndexedHeaderPaths::save(ConfigManager* config, const std::string& key)
{
	config->setValues(key + "/indexed_header_paths/indexed_header_path", getIndexedHeaderPaths());
}
