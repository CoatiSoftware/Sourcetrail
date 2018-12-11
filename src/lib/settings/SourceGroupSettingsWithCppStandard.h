#ifndef SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H

#include <memory>
#include <string>
#include <vector>

#include "SourceGroupSettingsBase.h"

class ConfigManager;

class SourceGroupSettingsWithCppStandard
	: virtual public SourceGroupSettingsBase
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
	std::wstring getDefaultCppStandard() const;

	std::wstring m_cppStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_CPP_STANDARD_H
