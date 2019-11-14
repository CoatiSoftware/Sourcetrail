#ifndef SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H
#define SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H

#include <vector>

#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithCStandard
	: public SourceGroupSettingsComponent
{
public:
	static std::wstring getDefaultCStandardStatic();

	virtual ~SourceGroupSettingsWithCStandard() = default;

	std::wstring getCStandard() const;
	void setCStandard(const std::wstring& standard);

	std::vector<std::wstring> getAvailableCStandards() const;

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	std::wstring getDefaultCStandard() const;

	std::wstring m_cStandard;
};

#endif // SOURCE_GROUP_SETTINGS_WITH_C_STANDARD_H
