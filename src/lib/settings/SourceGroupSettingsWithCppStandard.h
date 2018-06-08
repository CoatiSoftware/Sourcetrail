#ifndef SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H

#include <memory>
#include <string>
#include <vector>

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithCppStandard
{
public:
	static std::string getDefaultCppStandardStatic();

	SourceGroupSettingsWithCppStandard() = default;
	virtual ~SourceGroupSettingsWithCppStandard() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithCppStandard> other) const;

	std::string getCppStandard() const;
	void setCppStandard(const std::string& standard);

	std::vector<std::string> getAvailableCppStandards() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;
	std::string getDefaultCppStandard() const;

	std::string m_cppStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H
