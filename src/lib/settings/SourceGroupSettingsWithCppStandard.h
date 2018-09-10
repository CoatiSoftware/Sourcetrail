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
	static std::wstring getDefaultCppStandardStatic();

	SourceGroupSettingsWithCppStandard() = default;
	virtual ~SourceGroupSettingsWithCppStandard() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithCppStandard> other) const;

	std::wstring getCppStandard() const;
	void setCppStandard(const std::wstring& standard);

	std::vector<std::wstring> getAvailableCppStandards() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;
	std::wstring getDefaultCppStandard() const;

	std::wstring m_cppStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H
