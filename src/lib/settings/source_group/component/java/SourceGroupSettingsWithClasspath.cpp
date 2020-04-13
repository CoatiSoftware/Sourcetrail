#include "SourceGroupSettingsWithClasspath.h"

#include "../../../ProjectSettings.h"
#include "../../../../utility/utility.h"

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

bool SourceGroupSettingsWithClasspath::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithClasspath* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithClasspath*>(other);

	return (
		otherPtr && (m_useJreSystemLibrary == otherPtr->m_useJreSystemLibrary) &&
		utility::isPermutation(m_classpath, otherPtr->m_classpath));
}

void SourceGroupSettingsWithClasspath::load(const ConfigManager* config, const std::string& key)
{
	setClasspath(
		config->getValuesOrDefaults(key + "/class_paths/class_path", std::vector<FilePath>()));
	setUseJreSystemLibrary(config->getValueOrDefault(key + "/use_jre_system_library", true));
}

void SourceGroupSettingsWithClasspath::save(ConfigManager* config, const std::string& key)
{
	config->setValues(key + "/class_paths/class_path", getClasspath());
	config->setValue(key + "/use_jre_system_library", getUseJreSystemLibrary());
}
