#ifndef SOURCE_GROUP_SETTINGS_WITH_CXX_CDB_PATH_H
#define SOURCE_GROUP_SETTINGS_WITH_CXX_CDB_PATH_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "FilePath.h"
#include "SourceGroupSettingsComponent.h"

class SourceGroupSettingsWithCxxCdbPath
	: public SourceGroupSettingsComponent
{
public:
	virtual ~SourceGroupSettingsWithCxxCdbPath() = default;

	FilePath getCompilationDatabasePath() const;
	FilePath getCompilationDatabasePathExpandedAndAbsolute() const;
	void setCompilationDatabasePath(const FilePath& compilationDatabasePath);

protected:
	bool equals(const SourceGroupSettingsBase* other) const override;

	void load(const ConfigManager* config, const std::string& key) override;
	void save(ConfigManager* config, const std::string& key) override;

private:
	FilePath m_compilationDatabasePath;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_WITH_CXX_CDB_PATH_H
