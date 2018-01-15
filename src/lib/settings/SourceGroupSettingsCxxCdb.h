#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_H

#include "settings/SourceGroupSettingsCxx.h"

class SourceGroupSettingsCxxCdb
	: public SourceGroupSettingsCxx
{
public:
	SourceGroupSettingsCxxCdb(const std::string& id, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsCxxCdb();

	virtual void load(std::shared_ptr<const ConfigManager> config) override;
	virtual void save(std::shared_ptr<ConfigManager> config) override;

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getCompilationDatabasePath() const;
	FilePath getCompilationDatabasePathExpandedAndAbsolute() const;
	void setCompilationDatabasePath(const FilePath& compilationDatabasePath);

private:
	FilePath m_compilationDatabasePath;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_CDB_H
