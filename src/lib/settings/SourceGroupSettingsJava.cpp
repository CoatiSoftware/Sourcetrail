#include "settings/SourceGroupSettingsJava.h"

#include "utility/utility.h"

SourceGroupSettingsJava::SourceGroupSettingsJava(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, type, projectSettings)
	, m_useJreSystemLibrary(true)
	, m_classpath(std::vector<FilePath>())
{
}

SourceGroupSettingsJava::~SourceGroupSettingsJava()
{
}

void SourceGroupSettingsJava::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	setClasspath(getPathValues(key + "/class_paths/class_path", config));
	setUseJreSystemLibrary(getValue<bool>(key + "/use_jre_system_library", true, config));
}

void SourceGroupSettingsJava::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	setPathValues(key + "/class_paths/class_path", getClasspath(), config);
	setValue(key + "/use_jre_system_library", getUseJreSystemLibrary(), config);
}

bool SourceGroupSettingsJava::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJava> otherJava = std::dynamic_pointer_cast<SourceGroupSettingsJava>(other);

	return (
		otherJava &&
		SourceGroupSettings::equals(other) &&
		m_useJreSystemLibrary == otherJava->m_useJreSystemLibrary &&
		utility::isPermutation(m_classpath, otherJava->m_classpath)
	);
}

std::vector<std::string> SourceGroupSettingsJava::getAvailableLanguageStandards() const
{
	return std::vector<std::string>{"1", "2", "3", "4", "5", "6", "7", "8"};
}

bool SourceGroupSettingsJava::getUseJreSystemLibrary() const
{
	return m_useJreSystemLibrary;
}

void SourceGroupSettingsJava::setUseJreSystemLibrary(bool useJreSystemLibrary)
{
	m_useJreSystemLibrary = useJreSystemLibrary;
}

std::vector<FilePath> SourceGroupSettingsJava::getClasspath() const
{
	return m_classpath;
}

std::vector<FilePath> SourceGroupSettingsJava::getClasspathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathsExpandedAndAbsolute(getClasspath());
}

void SourceGroupSettingsJava::setClasspath(const std::vector<FilePath>& classpath)
{
	m_classpath = classpath;
}
std::vector<std::string> SourceGroupSettingsJava::getDefaultSourceExtensions() const
{
	return std::vector<std::string>(1, ".java");
}

std::string SourceGroupSettingsJava::getDefaultStandard() const
{
	return "8";
}
