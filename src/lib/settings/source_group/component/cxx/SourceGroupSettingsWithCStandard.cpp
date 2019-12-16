#include "SourceGroupSettingsWithCStandard.h"

#include "ProjectSettings.h"

std::wstring SourceGroupSettingsWithCStandard::getDefaultCStandardStatic()
{
#ifdef __linux__
	return L"gnu17";
#else
	return L"c17";
#endif
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
	// as defined in clang/include/clang/Frontend/LangStandards.def

	return {
		L"c2x",
		L"gnu2x",
		L"c17",
		L"gnu17",
		L"iso9899:2017",
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
		L"gnu89"};
}

bool SourceGroupSettingsWithCStandard::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCStandard* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCStandard*>(other);

	return (otherPtr && m_cStandard == otherPtr->m_cStandard);
}

void SourceGroupSettingsWithCStandard::load(const ConfigManager* config, const std::string& key)
{
	setCStandard(config->getValueOrDefault<std::wstring>(key + "/c_standard", L""));
}

void SourceGroupSettingsWithCStandard::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/c_standard", getCStandard());
}

std::wstring SourceGroupSettingsWithCStandard::getDefaultCStandard() const
{
	return getDefaultCStandardStatic();
}
