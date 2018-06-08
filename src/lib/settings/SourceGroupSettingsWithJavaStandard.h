#ifndef SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H

#include <memory>
#include <string>
#include <vector>

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithJavaStandard
{
public:
	static std::string getDefaultJavaStandardStatic();

	SourceGroupSettingsWithJavaStandard() = default;
	virtual ~SourceGroupSettingsWithJavaStandard() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithJavaStandard> other) const;

	std::string getJavaStandard() const;
	void setJavaStandard(const std::string& standard);

	std::vector<std::string> getAvailableJavaStandards() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;
	std::string getDefaultJavaStandard() const;

	std::string m_javaStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H
