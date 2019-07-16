#ifndef SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H

#include <memory>
#include <string>
#include <vector>

#include "SourceGroupSettingsBase.h"

class ConfigManager;

class SourceGroupSettingsWithJavaStandard
	: virtual public SourceGroupSettingsBase
{
public:
	static std::wstring getDefaultJavaStandardStatic();

	SourceGroupSettingsWithJavaStandard() = default;
	virtual ~SourceGroupSettingsWithJavaStandard() = default;

	bool equals(std::shared_ptr<SourceGroupSettingsWithJavaStandard> other) const;

	std::wstring getJavaStandard() const;
	void setJavaStandard(const std::wstring& standard);

	std::vector<std::wstring> getAvailableJavaStandards() const;

protected:
	void load(std::shared_ptr<const ConfigManager> config, const std::string& key);
	void save(std::shared_ptr<ConfigManager> config, const std::string& key);

private:
	std::wstring getDefaultJavaStandard() const;

	std::wstring m_javaStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_JAVA_STANDARD_H
