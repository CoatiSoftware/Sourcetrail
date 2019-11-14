#include "SourceGroupSettingsWithJavaStandard.h"

#include "ProjectSettings.h"

std::wstring SourceGroupSettingsWithJavaStandard::getDefaultJavaStandardStatic()
{
	return L"8";
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

bool SourceGroupSettingsWithJavaStandard::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithJavaStandard* otherPtr = dynamic_cast<const SourceGroupSettingsWithJavaStandard*>(other);

	return (
		otherPtr &&
		m_javaStandard == otherPtr->m_javaStandard
	);
}

void SourceGroupSettingsWithJavaStandard::load(const ConfigManager* config, const std::string& key)
{
	setJavaStandard(config->getValueOrDefault<std::wstring>(key + "/java_standard", L""));
}

void SourceGroupSettingsWithJavaStandard::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/java_standard", getJavaStandard());
}

std::wstring SourceGroupSettingsWithJavaStandard::getDefaultJavaStandard() const
{
	return getDefaultJavaStandardStatic();
}
