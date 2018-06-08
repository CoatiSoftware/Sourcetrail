#ifndef SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H

#include <memory>
#include <string>
#include <vector>

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithCStandard
{
public:
	static std::string getDefaultCStandardStatic();

	SourceGroupSettingsWithCStandard() = default;
	virtual ~SourceGroupSettingsWithCStandard() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithCStandard> other) const;

	std::string getCStandard() const;
	void setCStandard(const std::string& standard);

	std::vector<std::string> getAvailableCStandards() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;
	std::string getDefaultCStandard() const;

	std::string m_cStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H
