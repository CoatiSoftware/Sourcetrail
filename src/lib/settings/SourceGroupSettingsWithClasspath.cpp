#include "settings/SourceGroupSettingsWithClasspath.h"

#include "settings/ProjectSettings.h"
#include "utility/utility.h"

SourceGroupSettingsWithClasspath::SourceGroupSettingsWithClasspath()
	: m_classpath(std::vector<FilePath>())
	, m_useJreSystemLibrary(true)
{
}

bool SourceGroupSettingsWithClasspath::equals(std::shared_ptr<SourceGroupSettingsWithClasspath> other) const
{
	return (
		(m_useJreSystemLibrary == other->m_useJreSystemLibrary) &&
		utility::isPermutation(m_classpath, other->m_classpath)
	);
}

void SourceGroupSettingsWithClasspath::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setClasspath(config->getValuesOrDefaults(key + "/class_paths/class_path", std::vector<FilePath>()));
	setUseJreSystemLibrary(config->getValueOrDefault(key + "/use_jre_system_library", true));
}

void SourceGroupSettingsWithClasspath::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValues(key + "/class_paths/class_path", getClasspath());
	config->setValue(key + "/use_jre_system_library", getUseJreSystemLibrary());
}

std::vector<FilePath> SourceGroupSettingsWithClasspath::getClasspath() const
{
	return m_classpath;
}

std::vector<FilePath> SourceGroupSettingsWithClasspath::getClasspathExpandedAndAbsolute() const
{
	return getProjectSettings()->makePathsExpandedAndAbsolute(getClasspath());
}

void SourceGroupSettingsWithClasspath::setClasspath(const std::vector<FilePath>& classpath)
{
	m_classpath = classpath;
}

bool SourceGroupSettingsWithClasspath::getUseJreSystemLibrary() const
{
	return m_useJreSystemLibrary;
}

void SourceGroupSettingsWithClasspath::setUseJreSystemLibrary(bool useJreSystemLibrary)
{
	m_useJreSystemLibrary = useJreSystemLibrary;
}
