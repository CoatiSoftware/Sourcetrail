#include "SourceGroupSettingsJavaSonargraph.h"

SourceGroupSettingsJavaSonargraph::SourceGroupSettingsJavaSonargraph(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettings(id, SOURCE_GROUP_JAVA_SONARGRAPH, projectSettings)
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsJavaSonargraph::createCopy() const
{
	return std::make_shared<SourceGroupSettingsJavaSonargraph>(*this);
}

void SourceGroupSettingsJavaSonargraph::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettings::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithClasspath::load(config, key);
	SourceGroupSettingsWithJavaStandard::load(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::load(config, key);
}

void SourceGroupSettingsJavaSonargraph::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettings::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithClasspath::save(config, key);
	SourceGroupSettingsWithJavaStandard::save(config, key);
	SourceGroupSettingsWithSonargraphProjectPath::save(config, key);
}

bool SourceGroupSettingsJavaSonargraph::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsJavaSonargraph> otherJavaSonargraph = std::dynamic_pointer_cast<SourceGroupSettingsJavaSonargraph>(other);

	return (
		otherJavaSonargraph &&
		SourceGroupSettings::equals(other) &&
		SourceGroupSettingsWithClasspath::equals(otherJavaSonargraph) &&
		SourceGroupSettingsWithJavaStandard::equals(otherJavaSonargraph) &&
		SourceGroupSettingsWithSonargraphProjectPath::equals(otherJavaSonargraph)
	);
}

const ProjectSettings* SourceGroupSettingsJavaSonargraph::getProjectSettings() const
{
	return m_projectSettings;
}
