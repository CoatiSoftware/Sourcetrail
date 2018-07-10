#include "settings/SourceGroupSettingsWithJavaStandard.h"

#include "settings/ProjectSettings.h"

std::string SourceGroupSettingsWithJavaStandard::getDefaultJavaStandardStatic()
{
	return "8";
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
	setJavaStandard(config->getValueOrDefault<std::string>(key + "/java_standard", ""));
}

void SourceGroupSettingsWithJavaStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/java_standard", getJavaStandard());
}

std::string SourceGroupSettingsWithJavaStandard::getJavaStandard() const
{
	if (m_javaStandard.empty())
	{
		return getDefaultJavaStandard();
	}
	return m_javaStandard;
}

void SourceGroupSettingsWithJavaStandard::setJavaStandard(const std::string& standard)
{
	m_javaStandard = standard;
}

std::vector<std::string> SourceGroupSettingsWithJavaStandard::getAvailableJavaStandards() const
{
	return {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
}

std::string SourceGroupSettingsWithJavaStandard::getDefaultJavaStandard() const
{
	return getDefaultJavaStandardStatic();
}
