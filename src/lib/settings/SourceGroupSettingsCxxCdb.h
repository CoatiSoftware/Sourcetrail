#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_H

#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "FilePath.h"

class SourceGroupSettingsCxxCdb
	: public SourceGroupSettingsCxx
	, public SourceGroupSettingsWithExcludeFilters
	, public SourceGroupSettingsWithIndexedHeaderPaths
{
public:
	SourceGroupSettingsCxxCdb(const std::string& id, const ProjectSettings* projectSettings);

	std::shared_ptr<SourceGroupSettings> createCopy() const override;

	void load(std::shared_ptr<const ConfigManager> config) override;
	void save(std::shared_ptr<ConfigManager> config) override;

	bool equals(std::shared_ptr<SourceGroupSettings> other) const override;

	FilePath getCompilationDatabasePath() const;
	FilePath getCompilationDatabasePathExpandedAndAbsolute() const;
	void setCompilationDatabasePath(const FilePath& compilationDatabasePath);

private:
	FilePath m_compilationDatabasePath;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_CDB_H
