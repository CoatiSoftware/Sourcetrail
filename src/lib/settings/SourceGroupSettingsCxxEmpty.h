#ifndef SOURCE_GROUP_SETTINGS_CXX_EMPTY_H
#define SOURCE_GROUP_SETTINGS_CXX_EMPTY_H

#include "settings/SourceGroupSettingsCxx.h"

class SourceGroupSettingsCxxEmpty
	: public SourceGroupSettingsCxx
{
public:
	static std::vector<std::string> getAvailableArchTypes();
	static std::vector<std::string> getAvailableVendorTypes();
	static std::vector<std::string> getAvailableOsTypes();
	static std::vector<std::string> getAvailableEnvironmentTypes();

	SourceGroupSettingsCxxEmpty(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsCxxEmpty();

	virtual void load(std::shared_ptr<const ConfigManager> config);
	virtual void save(std::shared_ptr<ConfigManager> config);

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	bool getTargetOptionsEnabled() const;
	void setTargetOptionsEnabled(bool targetOptionsEnabled);

	std::string getTargetArch() const;
	void setTargetArch(const std::string& arch);

	std::string getTargetVendor() const;
	void setTargetVendor(const std::string& vendor);

	std::string getTargetSys() const;
	void setTargetSys(const std::string& sys);

	std::string getTargetAbi() const;
	void setTargetAbi(const std::string& abi);

	std::string getTargetFlag() const;

private:
	bool m_targetOptionsEnabled;
	std::string m_targetArch;
	std::string m_targetVendor;
	std::string m_targetSys;
	std::string m_targetAbi;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_EMPTY_H
