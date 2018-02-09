#ifndef SOURCE_GROUP_SETTINGS_CXX_EMPTY_H
#define SOURCE_GROUP_SETTINGS_CXX_EMPTY_H

#include "settings/SourceGroupSettingsCxx.h"

class SourceGroupSettingsCxxEmpty
	: public SourceGroupSettingsCxx
{
public:
	static std::vector<std::wstring> getAvailableArchTypes();
	static std::vector<std::wstring> getAvailableVendorTypes();
	static std::vector<std::wstring> getAvailableOsTypes();
	static std::vector<std::wstring> getAvailableEnvironmentTypes();

	SourceGroupSettingsCxxEmpty(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsCxxEmpty();

	virtual void load(std::shared_ptr<const ConfigManager> config) override;
	virtual void save(std::shared_ptr<ConfigManager> config) override;

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

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

private:
	bool m_targetOptionsEnabled;
	std::wstring m_targetArch;
	std::wstring m_targetVendor;
	std::wstring m_targetSys;
	std::wstring m_targetAbi;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_EMPTY_H
