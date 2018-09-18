#ifndef SOURCE_GROUP_SETTINGS_WITH_CLASSPATH_H
#define SOURCE_GROUP_SETTINGS_WITH_CLASSPATH_H

#include <memory>
#include <string>
#include <vector>

#include "FilePath.h"

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithClasspath
{
public:
	SourceGroupSettingsWithClasspath();
	virtual ~SourceGroupSettingsWithClasspath() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithClasspath> other) const;

	std::vector<FilePath> getClasspath() const;
	std::vector<FilePath> getClasspathExpandedAndAbsolute() const;
	void setClasspath(const std::vector<FilePath>& classpath);

	bool getUseJreSystemLibrary() const;
	void setUseJreSystemLibrary(bool useJreSystemLibrary);

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;

	std::vector<FilePath> m_classpath;
	bool m_useJreSystemLibrary;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CLASSPATH_H
