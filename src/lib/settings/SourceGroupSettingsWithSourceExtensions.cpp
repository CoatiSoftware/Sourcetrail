#include "settings/SourceGroupSettingsWithSourceExtensions.h"

#include "settings/ProjectSettings.h"
#include "utility/utility.h"

SourceGroupSettingsWithSourceExtensions::SourceGroupSettingsWithSourceExtensions()
	: m_sourceExtensions(std::vector<std::wstring>())
{
}

bool SourceGroupSettingsWithSourceExtensions::equals(std::shared_ptr<SourceGroupSettingsWithSourceExtensions> other) const
{
	return utility::isPermutation(m_sourceExtensions, other->m_sourceExtensions);
}

void SourceGroupSettingsWithSourceExtensions::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setSourceExtensions(config->getValuesOrDefaults(key + "/source_extensions/source_extension", std::vector<std::wstring>()));
}

void SourceGroupSettingsWithSourceExtensions::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/source_extensions/source_extension", getSourceExtensions());
}

std::vector<std::wstring> SourceGroupSettingsWithSourceExtensions::getSourceExtensions() const
{
	if (m_sourceExtensions.empty())
	{
		return getDefaultSourceExtensions();
	}
	return m_sourceExtensions;
}

void SourceGroupSettingsWithSourceExtensions::setSourceExtensions(const std::vector<std::wstring>& sourceExtensions)
{
	m_sourceExtensions = sourceExtensions;
}
