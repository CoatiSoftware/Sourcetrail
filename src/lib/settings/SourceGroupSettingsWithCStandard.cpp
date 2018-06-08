#include "settings/SourceGroupSettingsWithCStandard.h"

#include "settings/ProjectSettings.h"

std::string SourceGroupSettingsWithCStandard::getDefaultCStandardStatic()
{
	return "c11";
}

bool SourceGroupSettingsWithCStandard::equals(std::shared_ptr<SourceGroupSettingsWithCStandard> other) const
{
	return (
		other &&
		m_cStandard == other->m_cStandard
	);
}

void SourceGroupSettingsWithCStandard::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setCStandard(config->getValueOrDefault<std::string>(key + "/c_standard", ""));
}

void SourceGroupSettingsWithCStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/c_standard", getCStandard());
}

std::string SourceGroupSettingsWithCStandard::getCStandard() const
{
	if (m_cStandard.empty())
	{
		return getDefaultCStandard();
	}
	return m_cStandard;
}

void SourceGroupSettingsWithCStandard::setCStandard(const std::string& standard)
{
	m_cStandard = standard;
}

std::vector<std::string> SourceGroupSettingsWithCStandard::getAvailableCStandards() const
{
	return {
		"c11",
		"gnu11",
		"iso9899:2011",
		"c99",
		"gnu99",
		"iso9899:1999",
		"iso9899:199409",
		"c90",
		"gnu90",
		"iso9899:1990",
		"c89",
		"gnu89"
	};
}

std::string SourceGroupSettingsWithCStandard::getDefaultCStandard() const
{
	return getDefaultCStandardStatic();
}
