#include "settings/SourceGroupSettingsWithCStandard.h"

#include "settings/ProjectSettings.h"

std::wstring SourceGroupSettingsWithCStandard::getDefaultCStandardStatic()
{
	return L"c11";
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
	setCStandard(config->getValueOrDefault<std::wstring>(key + "/c_standard", L""));
}

void SourceGroupSettingsWithCStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/c_standard", getCStandard());
}

std::wstring SourceGroupSettingsWithCStandard::getCStandard() const
{
	if (m_cStandard.empty())
	{
		return getDefaultCStandard();
	}
	return m_cStandard;
}

void SourceGroupSettingsWithCStandard::setCStandard(const std::wstring& standard)
{
	m_cStandard = standard;
}

std::vector<std::wstring> SourceGroupSettingsWithCStandard::getAvailableCStandards() const
{
	return {
		L"c11",
		L"gnu11",
		L"iso9899:2011",
		L"c99",
		L"gnu99",
		L"iso9899:1999",
		L"iso9899:199409",
		L"c90",
		L"gnu90",
		L"iso9899:1990",
		L"c89",
		L"gnu89"
	};
}

std::wstring SourceGroupSettingsWithCStandard::getDefaultCStandard() const
{
	return getDefaultCStandardStatic();
}
