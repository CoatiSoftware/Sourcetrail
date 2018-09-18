#include "SourceGroupSettingsWithJavaStandard.h"

#include "ProjectSettings.h"

std::wstring SourceGroupSettingsWithJavaStandard::getDefaultJavaStandardStatic()
{
	return L"8";
}

bool SourceGroupSettingsWithJavaStandard::equals(std::shared_ptr<SourceGroupSettingsWithJavaStandard> other) const
{
	return (
		other &&
		m_javaStandard == other->m_javaStandard
	);
}

void SourceGroupSettingsWithJavaStandard::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setJavaStandard(config->getValueOrDefault<std::wstring>(key + "/java_standard", L""));
}

void SourceGroupSettingsWithJavaStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/java_standard", getJavaStandard());
}

std::wstring SourceGroupSettingsWithJavaStandard::getJavaStandard() const
{
	if (m_javaStandard.empty())
	{
		return getDefaultJavaStandard();
	}
	return m_javaStandard;
}

void SourceGroupSettingsWithJavaStandard::setJavaStandard(const std::wstring& standard)
{
	m_javaStandard = standard;
}

std::vector<std::wstring> SourceGroupSettingsWithJavaStandard::getAvailableJavaStandards() const
{
	return {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};
}

std::wstring SourceGroupSettingsWithJavaStandard::getDefaultJavaStandard() const
{
	return getDefaultJavaStandardStatic();
}
