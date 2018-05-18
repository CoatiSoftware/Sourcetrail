#include "settings/SourceGroupSettingsCxxCdb.h"

#include "settings/ProjectSettings.h"
#include "utility/ConfigManager.h"

SourceGroupSettingsCxxCdb::SourceGroupSettingsCxxCdb(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_CXX_CDB, projectSettings)
	, m_compilationDatabasePath(FilePath())
{
}

void SourceGroupSettingsCxxCdb::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::load(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::load(config, key);

	setCompilationDatabasePath(config->getValueOrDefault(key + "/build_file_path/compilation_db_path", FilePath(L"")));
}

void SourceGroupSettingsCxxCdb::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	SourceGroupSettingsWithExcludeFilters::save(config, key);
	SourceGroupSettingsWithIndexedHeaderPaths::save(config, key);

	config->setValue(key + "/build_file_path/compilation_db_path", getCompilationDatabasePath().wstr());
}

bool SourceGroupSettingsCxxCdb::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxxCdb> otherCxxCdb = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(other);

	return (
		otherCxxCdb &&
		SourceGroupSettingsCxx::equals(other) &&
		SourceGroupSettingsWithExcludeFilters::equals(otherCxxCdb) &&
		SourceGroupSettingsWithIndexedHeaderPaths::equals(otherCxxCdb) &&
		m_compilationDatabasePath == otherCxxCdb->m_compilationDatabasePath
	);
}

FilePath SourceGroupSettingsCxxCdb::getCompilationDatabasePath() const
{
	return m_compilationDatabasePath;
}

FilePath SourceGroupSettingsCxxCdb::getCompilationDatabasePathExpandedAndAbsolute() const
{
	return m_projectSettings->makePathExpandedAndAbsolute(getCompilationDatabasePath());
}

void SourceGroupSettingsCxxCdb::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	m_compilationDatabasePath = compilationDatabasePath;
}

const ProjectSettings* SourceGroupSettingsCxxCdb::getProjectSettings() const
{
	return m_projectSettings;
}
