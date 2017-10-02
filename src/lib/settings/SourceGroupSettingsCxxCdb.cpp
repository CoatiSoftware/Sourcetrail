#include "settings/SourceGroupSettingsCxxCdb.h"

#include "utility/utility.h"
#include "utility/utilityApp.h"

SourceGroupSettingsCxxCdb::SourceGroupSettingsCxxCdb(const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettingsCxx(id, SOURCE_GROUP_CXX_CDB, projectSettings)
	, m_compilationDatabasePath(FilePath())
{
}

SourceGroupSettingsCxxCdb::~SourceGroupSettingsCxxCdb()
{
}

void SourceGroupSettingsCxxCdb::load(std::shared_ptr<const ConfigManager> config)
{
	SourceGroupSettingsCxx::load(config);

	const std::string key = s_keyPrefix + getId();

	setCompilationDatabasePath(FilePath(getValue<std::string>(key + "/build_file_path/compilation_db_path", "", config)));
}

void SourceGroupSettingsCxxCdb::save(std::shared_ptr<ConfigManager> config)
{
	SourceGroupSettingsCxx::save(config);

	const std::string key = s_keyPrefix + getId();

	setValue(key + "/build_file_path/compilation_db_path", getCompilationDatabasePath().str(), config);
}

bool SourceGroupSettingsCxxCdb::equals(std::shared_ptr<SourceGroupSettings> other) const
{
	std::shared_ptr<SourceGroupSettingsCxxCdb> otherCxxCdb = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(other);

	return (
		otherCxxCdb &&
		SourceGroupSettingsCxx::equals(other) &&
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
