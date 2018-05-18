#include "settings/SourceGroupSettingsWithIndexedHeaderPaths.h"

#include "settings/ProjectSettings.h"
#include "utility/utility.h"

SourceGroupSettingsWithIndexedHeaderPaths::SourceGroupSettingsWithIndexedHeaderPaths()
	: m_indexedHeaderPaths(std::vector<FilePath>())
{
}

bool SourceGroupSettingsWithIndexedHeaderPaths::equals(std::shared_ptr<SourceGroupSettingsWithIndexedHeaderPaths> other) const
{
	return (
		utility::isPermutation(m_indexedHeaderPaths, other->m_indexedHeaderPaths)
	);
}

void SourceGroupSettingsWithIndexedHeaderPaths::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setIndexedHeaderPaths(config->getValuesOrDefaults(key + "/indexed_header_paths/indexed_header_path", std::vector<FilePath>()));
}

void SourceGroupSettingsWithIndexedHeaderPaths::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/indexed_header_paths/indexed_header_path", getIndexedHeaderPaths());
}

std::vector<FilePath> SourceGroupSettingsWithIndexedHeaderPaths::getIndexedHeaderPaths() const
{
	return m_indexedHeaderPaths;
}

std::vector<FilePath> SourceGroupSettingsWithIndexedHeaderPaths::getIndexedHeaderPathsExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getIndexedHeaderPaths());
}

void SourceGroupSettingsWithIndexedHeaderPaths::setIndexedHeaderPaths(const std::vector<FilePath>& indexedHeaderPaths)
{
	m_indexedHeaderPaths = indexedHeaderPaths;
}
