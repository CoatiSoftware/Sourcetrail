#ifndef SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
#define SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H

#include "SourceGroupSettingsJava.h"
#include "SourceGroupSettingsWithClasspath.h"
#include "SourceGroupSettingsWithSourcePaths.h"

class SourceGroupSettingsJavaEmpty
	: public SourceGroupSettingsJava
	, public SourceGroupSettingsWithSourcePaths
	, public SourceGroupSettingsWithClasspath
{
public:
	SourceGroupSettingsJavaEmpty(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;
};

#endif // SOURCE_GROUP_SETTINGS_JAVA_EMPTY_H
