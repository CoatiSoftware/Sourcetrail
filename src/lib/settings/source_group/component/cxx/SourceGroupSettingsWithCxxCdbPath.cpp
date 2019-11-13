#include "SourceGroupSettingsWithCxxCdbPath.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "ProjectSettings.h"
#include "utilityFile.h"

FilePath SourceGroupSettingsWithCxxCdbPath::getCompilationDatabasePath() const
{
	return m_compilationDatabasePath;
}

FilePath SourceGroupSettingsWithCxxCdbPath::getCompilationDatabasePathExpandedAndAbsolute() const
{
	return utility::getExpandedAndAbsolutePath(
		getCompilationDatabasePath(), getProjectSettings()->getProjectDirectoryPath());
}

void SourceGroupSettingsWithCxxCdbPath::setCompilationDatabasePath(const FilePath& compilationDatabasePath)
{
	m_compilationDatabasePath = compilationDatabasePath;
}

bool SourceGroupSettingsWithCxxCdbPath::equals(const SourceGroupSettingsBase* other) const
{
	const SourceGroupSettingsWithCxxCdbPath* otherPtr =
		dynamic_cast<const SourceGroupSettingsWithCxxCdbPath*>(other);

	return (
		otherPtr &&
		m_compilationDatabasePath == otherPtr->m_compilationDatabasePath
	);
}

void SourceGroupSettingsWithCxxCdbPath::load(const ConfigManager* config, const std::string& key)
{
	setCompilationDatabasePath(config->getValueOrDefault(key + "/build_file_path/compilation_db_path", FilePath(L"")));
}

void SourceGroupSettingsWithCxxCdbPath::save(ConfigManager* config, const std::string& key)
{
	config->setValue(key + "/build_file_path/compilation_db_path", getCompilationDatabasePath().wstr());
}

#endif // BUILD_CXX_LANGUAGE_PACKAGE
