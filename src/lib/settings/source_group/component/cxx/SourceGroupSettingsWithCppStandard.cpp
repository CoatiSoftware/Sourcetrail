#include "SourceGroupSettingsWithCppStandard.h"

#include "ProjectSettings.h"

std::wstring SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic()
{
#ifdef __linux__
	return L"gnu++17";
#else
	return L"c++17";
#endif
}

std::wstring SourceGroupSettingsWithCppStandard::getCppStandard() const
{
	if (m_cppStandard.empty())
	{
		return getDefaultCppStandard();
	}
	return m_cppStandard;
}

void SourceGroupSettingsWithCppStandard::setCppStandard(const std::wstring& standard)
{
	m_cppStandard = standard;
}

std::vector<std::wstring> SourceGroupSettingsWithCppStandard::getAvailableCppStandards() const
{
	// as defined in clang/include/clang/Frontend/LangStandards.def

	return {
		L"c++2a",
		L"gnu++2a",
		L"c++17",
		L"gnu++17",
		L"c++14",
		L"gnu++14",
		L"c++11",
		L"gnu++11",
		L"c++03",
		L"gnu++03",
		L"c++98",
		L"gnu++98"};
}

bool SourceGroupSettingsWithCppStandard::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCppStandard* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCppStandard*>(other);

	return (otherPtr && m_cppStandard == otherPtr->m_cppStandard);
}

void SourceGroupSettingsWithCppStandard::load(const ConfigManager* config, const std::string& key)
{
	setCppStandard(config->getValueOrDefault<std::wstring>(key + "/cpp_standard", L""));
}

void SourceGroupSettingsWithCppStandard::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/cpp_standard", getCppStandard());
}

std::wstring SourceGroupSettingsWithCppStandard::getDefaultCppStandard() const
{
	return getDefaultCppStandardStatic();
}
