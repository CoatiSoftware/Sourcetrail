#ifndef SOURCE_GROUP_SETTINGS_WITH_CXX_CROSS_COMPILATION_OPTIONS_H
#define SOURCE_GROUP_SETTINGS_WITH_CXX_CROSS_COMPILATION_OPTIONS_H

#include <memory>
#include <string>
#include <vector>

class ConfigManager;
class ProjectSettings;

class SourceGroupSettingsWithCxxCrossCompilationOptions
{
public:
	static std::vector<std::wstring> getAvailableArchTypes();
	static std::vector<std::wstring> getAvailableVendorTypes();
	static std::vector<std::wstring> getAvailableOsTypes();
	static std::vector<std::wstring> getAvailableEnvironmentTypes();

	SourceGroupSettingsWithCxxCrossCompilationOptions();
	virtual ~SourceGroupSettingsWithCxxCrossCompilationOptions() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithCxxCrossCompilationOptions> other) const;

	bool getTargetOptionsEnabled() const;
	void setTargetOptionsEnabled(bool targetOptionsEnabled);

	std::wstring getTargetArch() const;
	void setTargetArch(const std::wstring& arch);

	std::wstring getTargetVendor() const;
	void setTargetVendor(const std::wstring& vendor);

	std::wstring getTargetSys() const;
	void setTargetSys(const std::wstring& sys);

	std::wstring getTargetAbi() const;
	void setTargetAbi(const std::wstring& abi);

	std::wstring getTargetFlag() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	virtual const ProjectSettings* getProjectSettings() const = 0;

	bool m_targetOptionsEnabled;
	std::wstring m_targetArch;
	std::wstring m_targetVendor;
	std::wstring m_targetSys;
	std::wstring m_targetAbi;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CXX_CROSS_COMPILATION_OPTIONS_H
