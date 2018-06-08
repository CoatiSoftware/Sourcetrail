#include "settings/SourceGroupSettingsWithCppStandard.h"

#include "settings/ProjectSettings.h"

std::string SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic()
{
	return "c++17";
}

bool SourceGroupSettingsWithCppStandard::equals(std::shared_ptr<SourceGroupSettingsWithCppStandard> other) const
{
	return (
		other &&
		m_cppStandard == other->m_cppStandard
	);
}

void SourceGroupSettingsWithCppStandard::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setCppStandard(config->getValueOrDefault<std::string>(key + "/cpp_standard", ""));
}

void SourceGroupSettingsWithCppStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/cpp_standard", getCppStandard());
}

std::string SourceGroupSettingsWithCppStandard::getCppStandard() const
{
	if (m_cppStandard.empty())
	{
		return getDefaultCppStandard();
	}
	return m_cppStandard;
}

void SourceGroupSettingsWithCppStandard::setCppStandard(const std::string& standard)
{
	m_cppStandard = standard;
}

std::vector<std::string> SourceGroupSettingsWithCppStandard::getAvailableCppStandards() const
{
	return {
		"c++2a",
		"gnu++2a",
		"c++17",
		"gnu++17",
		"c++14",
		"gnu++14",
		"c++11",
		"gnu++11",
		"c++03",
		"gnu++03",
		"c++98",
		"gnu++98"
	};
}

std::string SourceGroupSettingsWithCppStandard::getDefaultCppStandard() const
{
	return getDefaultCppStandardStatic();
}
