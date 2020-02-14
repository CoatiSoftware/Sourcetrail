#include "ProjectSettings.h"

#include "SettingsMigrationDeleteKey.h"
#include "SettingsMigrationLambda.h"
#include "SettingsMigrationMoveKey.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SourceGroupSettingsUnloadable.h"
#include "logging.h"
#include "utilityFile.h"
#include "utilityString.h"
#include "utilityUuid.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#	include "SourceGroupSettingsCEmpty.h"
#	include "SourceGroupSettingsCppEmpty.h"
#	include "SourceGroupSettingsCxxCdb.h"
#	include "SourceGroupSettingsCxxCodeblocks.h"
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#	include "SourceGroupSettingsJavaEmpty.h"
#	include "SourceGroupSettingsJavaGradle.h"
#	include "SourceGroupSettingsJavaMaven.h"
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
#	include "SourceGroupSettingsPythonEmpty.h"
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

const std::wstring ProjectSettings::PROJECT_FILE_EXTENSION = L".srctrlprj";
const std::wstring ProjectSettings::BOOKMARK_DB_FILE_EXTENSION = L".srctrlbm";
const std::wstring ProjectSettings::INDEX_DB_FILE_EXTENSION = L".srctrldb";
const std::wstring ProjectSettings::TEMP_INDEX_DB_FILE_EXTENSION = L".srctrldb_tmp";

const size_t ProjectSettings::VERSION = 8;

LanguageType ProjectSettings::getLanguageOfProject(const FilePath& filePath)
{
	LanguageType languageType = LANGUAGE_UNKNOWN;

	ProjectSettings projectSettings;
	projectSettings.load(filePath);
	for (const std::shared_ptr<SourceGroupSettings>& sourceGroupSettings:
		 projectSettings.getAllSourceGroupSettings())
	{
		const LanguageType currentLanguageType = getLanguageTypeForSourceGroupType(
			sourceGroupSettings->getType());
		if (languageType == LANGUAGE_UNKNOWN)
		{
			languageType = currentLanguageType;
		}
		else if (languageType != currentLanguageType)
		{
			// language is unknown if source groups have different languages.
			languageType = LANGUAGE_UNKNOWN;
			break;
		}
	}

	return languageType;
}

ProjectSettings::ProjectSettings() {}

ProjectSettings::ProjectSettings(const FilePath& projectFilePath)
{
	setFilePath(projectFilePath);
}

ProjectSettings::~ProjectSettings() {}

bool ProjectSettings::equalsExceptNameAndLocation(const ProjectSettings& other) const
{
	const std::vector<std::shared_ptr<SourceGroupSettings>> allMySettings =
		getAllSourceGroupSettings();
	const std::vector<std::shared_ptr<SourceGroupSettings>> allOtherSettings =
		other.getAllSourceGroupSettings();

	if (allMySettings.size() != allOtherSettings.size())
	{
		return false;
	}

	for (const std::shared_ptr<SourceGroupSettings>& mySourceGroup: allMySettings)
	{
		bool matched = false;
		for (const std::shared_ptr<SourceGroupSettings>& otherSourceGroup: allOtherSettings)
		{
			if (mySourceGroup->equalsSettings(otherSourceGroup.get()))
			{
				matched = true;
				break;
			}
		}

		if (!matched)
		{
			return false;
		}
	}

	return true;
}

bool ProjectSettings::needMigration() const
{
	if (getVersion() == 0)
	{
		return false;
	}
	return getMigrations().willMigrate(this, ProjectSettings::VERSION);
}

void ProjectSettings::migrate()
{
	SettingsMigrator migrator = getMigrations();
	bool migrated = migrator.migrate(this, ProjectSettings::VERSION);

	if (migrated)
	{
		save();
	}
}

bool ProjectSettings::reload()
{
	return Settings::load(getFilePath());
}

FilePath ProjectSettings::getProjectFilePath() const
{
	return getFilePath();
}

void ProjectSettings::setProjectFilePath(std::wstring projectName, const FilePath& projectFileLocation)
{
	setFilePath(projectFileLocation.getConcatenated(L"/" + projectName + PROJECT_FILE_EXTENSION));
}

FilePath ProjectSettings::getDependenciesDirectoryPath() const
{
	return getProjectDirectoryPath().concatenate(L"sourcetrail_dependencies");
}

FilePath ProjectSettings::getDBFilePath() const
{
	return getFilePath().replaceExtension(INDEX_DB_FILE_EXTENSION);
}

FilePath ProjectSettings::getTempDBFilePath() const
{
	return getFilePath().replaceExtension(TEMP_INDEX_DB_FILE_EXTENSION);
}

FilePath ProjectSettings::getBookmarkDBFilePath() const
{
	return getFilePath().replaceExtension(BOOKMARK_DB_FILE_EXTENSION);
}

std::wstring ProjectSettings::getProjectName() const
{
	return getFilePath().withoutExtension().fileName();
}

FilePath ProjectSettings::getProjectDirectoryPath() const
{
	return getFilePath().getParentDirectory();
}

std::string ProjectSettings::getDescription() const
{
	return getValue<std::string>("description", "");
}

std::vector<std::shared_ptr<SourceGroupSettings>> ProjectSettings::getAllSourceGroupSettings() const
{
	std::vector<std::shared_ptr<SourceGroupSettings>> allSettings;
	for (const std::string& key: m_config->getSublevelKeys("source_groups"))
	{
		const std::string id = key.substr(std::string(SourceGroupSettings::s_keyPrefix).length());
		const SourceGroupType type = stringToSourceGroupType(
			getValue<std::string>(key + "/type", ""));

		std::shared_ptr<SourceGroupSettings> settings;

		switch (type)
		{
#if BUILD_CXX_LANGUAGE_PACKAGE
		case SOURCE_GROUP_C_EMPTY:
			settings = std::make_shared<SourceGroupSettingsCEmpty>(id, this);
			break;
		case SOURCE_GROUP_CPP_EMPTY:
			settings = std::make_shared<SourceGroupSettingsCppEmpty>(id, this);
			break;
		case SOURCE_GROUP_CXX_CDB:
			settings = std::make_shared<SourceGroupSettingsCxxCdb>(id, this);
			break;
		case SOURCE_GROUP_CXX_CODEBLOCKS:
			settings = std::make_shared<SourceGroupSettingsCxxCodeblocks>(id, this);
			break;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
		case SOURCE_GROUP_JAVA_EMPTY:
			settings = std::make_shared<SourceGroupSettingsJavaEmpty>(id, this);
			break;
		case SOURCE_GROUP_JAVA_MAVEN:
			settings = std::make_shared<SourceGroupSettingsJavaMaven>(id, this);
			break;
		case SOURCE_GROUP_JAVA_GRADLE:
			settings = std::make_shared<SourceGroupSettingsJavaGradle>(id, this);
			break;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
		case SOURCE_GROUP_PYTHON_EMPTY:
			settings = std::make_shared<SourceGroupSettingsPythonEmpty>(id, this);
			break;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
		case SOURCE_GROUP_CUSTOM_COMMAND:
			settings = std::make_shared<SourceGroupSettingsCustomCommand>(id, this);
			break;
		default:
			settings = std::make_shared<SourceGroupSettingsUnloadable>(id, this);
		}

		if (settings)
		{
			settings->loadSettings(m_config.get());
			allSettings.push_back(settings);
		}
		else
		{
			LOG_WARNING("Sourcegroup with id \"" + id + "\" could not be loaded.");
		}
	}

	return allSettings;
}

void ProjectSettings::setAllSourceGroupSettings(
	const std::vector<std::shared_ptr<SourceGroupSettings>>& allSettings)
{
	for (const std::string& key: m_config->getSublevelKeys("source_groups"))
	{
		m_config->removeValues(key);
	}

	for (const std::shared_ptr<SourceGroupSettings>& settings: allSettings)
	{
		const std::string key = SourceGroupSettings::s_keyPrefix + settings->getId();
		const SourceGroupType type = settings->getType();
		setValue(key + "/type", sourceGroupTypeToString(type));

		settings->saveSettings(m_config.get());
	}
}

std::vector<FilePath> ProjectSettings::makePathsExpandedAndAbsolute(const std::vector<FilePath>& paths) const
{
	std::vector<FilePath> p = utility::getExpandedPaths(paths);

	std::vector<FilePath> absPaths;
	const FilePath basePath = getProjectDirectoryPath();
	for (const FilePath& path: p)
	{
		if (path.isAbsolute())
		{
			absPaths.push_back(path);
		}
		else
		{
			absPaths.push_back(basePath.getConcatenated(path).makeCanonical());
		}
	}

	return absPaths;
}

FilePath ProjectSettings::makePathExpandedAndAbsolute(const FilePath& path) const
{
	return utility::getExpandedAndAbsolutePath(path, getProjectDirectoryPath());
}

SettingsMigrator ProjectSettings::getMigrations() const
{
	SettingsMigrator migrator;
	migrator.addMigration(
		1,
		std::make_shared<SettingsMigrationLambda>([](const SettingsMigration* migration,
													 Settings* settings) {
			const std::string language = migration->getValueFromSettings<std::string>(
				settings, "language_settings/language", "");
			const std::string standard = migration->getValueFromSettings<std::string>(
				settings, "language_settings/standard", "");

			if (language == "C" && !utility::isPrefix<std::string>("c", standard))
			{
				migration->setValueInSettings(settings, "language_settings/standard", "c" + standard);
			}

			if (language == "C++" && !utility::isPrefix<std::string>("c++", standard))
			{
				migration->setValueInSettings(
					settings, "language_settings/standard", "c++" + standard);
			}
		}));

	{
		const std::string sourceGroupKey = "source_groups/source_group_" + utility::getUuidString();

		migrator.addMigration(
			2, std::make_shared<SettingsMigrationMoveKey>("info/description", "description"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"language_settings/standard", sourceGroupKey + "/standard"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/source_paths/source_path", sourceGroupKey + "/source_paths/source_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/exclude_paths/exclude_path", sourceGroupKey + "/exclude_paths/exclude_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/extensions/source_extensions",
				sourceGroupKey + "/source_extensions/source_extension"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/header_search_paths/header_search_path",
				sourceGroupKey + "/header_search_paths/header_search_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/use_source_paths_for_header_search",
				sourceGroupKey + "/use_source_paths_for_header_search"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/framework_search_paths/framework_search_path",
				sourceGroupKey + "/framework_search_paths/framework_search_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/compiler_flags/compiler_flag",
				sourceGroupKey + "/compiler_flags/compiler_flag"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/build_file_path/compilation_db_path",
				sourceGroupKey + "/build_file_path/compilation_db_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/class_paths/class_path", sourceGroupKey + "/class_paths/class_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/maven/project_file_path", sourceGroupKey + "/maven/project_file_path"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/maven/dependencies_directory",
				sourceGroupKey + "/maven/dependencies_directory"));
		migrator.addMigration(
			2,
			std::make_shared<SettingsMigrationMoveKey>(
				"source/maven/should_index_tests", sourceGroupKey + "/maven/should_index_tests"));

		migrator.addMigration(
			3,
			std::make_shared<SettingsMigrationLambda>(
				[=](const SettingsMigration* migration, Settings* settings) {
					const std::string language = migration->getValueFromSettings<std::string>(
						settings, "language_settings/language", "");

					SourceGroupType type = SOURCE_GROUP_UNKNOWN;
#if BUILD_CXX_LANGUAGE_PACKAGE
					if (language == "C" || language == "C++")
					{
						const std::string cdbPath = migration->getValueFromSettings<std::string>(
							settings, sourceGroupKey + "/build_file_path/compilation_db_path", "");
						if (!cdbPath.empty())
						{
							type = SOURCE_GROUP_CXX_CDB;
						}
						else if (language == "C")
						{
							type = SOURCE_GROUP_C_EMPTY;
						}
						else
						{
							type = SOURCE_GROUP_CPP_EMPTY;
						}
					}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
					if (language == "Java")
					{
						const std::string mavenProjectFilePath =
							migration->getValueFromSettings<std::string>(
								settings, sourceGroupKey + "/maven/project_file_path", "");
						if (!mavenProjectFilePath.empty())
						{
							type = SOURCE_GROUP_JAVA_MAVEN;
						}
						const std::string gradleProjectFilePath =
							migration->getValueFromSettings<std::string>(
								settings, sourceGroupKey + "/gradle/project_file_path", "");
						if (!gradleProjectFilePath.empty())
						{
							type = SOURCE_GROUP_JAVA_GRADLE;
						}
						else
						{
							type = SOURCE_GROUP_JAVA_EMPTY;
						}
					}
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
					migration->setValueInSettings(
						settings, sourceGroupKey + "/type", sourceGroupTypeToString(type));
				}));
	}
	migrator.addMigration(
		4, std::make_shared<SettingsMigrationDeleteKey>("language_settings/language"));
	migrator.addMigration(
		4, std::make_shared<SettingsMigrationDeleteKey>("source/build_file_path/vs_solution_path"));
	migrator.addMigration(
		4, std::make_shared<SettingsMigrationDeleteKey>("source/extensions/header_extensions"));

	for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: getAllSourceGroupSettings())
	{
		const std::string key = SourceGroupSettings::s_keyPrefix + sourceGroupSettings->getId();
		migrator.addMigration(
			5,
			std::make_shared<SettingsMigrationMoveKey>(
				key + "/exclude_paths/exclude_path", key + "/exclude_filters/exclude_filter"));
	}

	for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: getAllSourceGroupSettings())
	{
#if BUILD_CXX_LANGUAGE_PACKAGE
		if (sourceGroupSettings->getType() == SOURCE_GROUP_CXX_CDB)
		{
			const std::string key = SourceGroupSettings::s_keyPrefix + sourceGroupSettings->getId();
			migrator.addMigration(
				6,
				std::make_shared<SettingsMigrationMoveKey>(
					key + "/source_paths/source_path",
					key + "/indexed_header_paths/indexed_header_path"));
		}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
	}

	for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: getAllSourceGroupSettings())
	{
		std::string languageName;
		switch (getLanguageTypeForSourceGroupType(sourceGroupSettings->getType()))
		{
#if BUILD_CXX_LANGUAGE_PACKAGE
		case LANGUAGE_C:
			languageName = "c";
			break;
		case LANGUAGE_CPP:
			languageName = "cpp";
			break;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
		case LANGUAGE_JAVA:
			languageName = "java";
			break;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
		case LANGUAGE_PYTHON:
			continue;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

		default:
			continue;
		}

		std::string key = SourceGroupSettings::s_keyPrefix + sourceGroupSettings->getId();
		migrator.addMigration(
			7,
			std::make_shared<SettingsMigrationMoveKey>(
				key + "/standard", key + "/" + languageName + "_standard"));
	}

	for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: getAllSourceGroupSettings())
	{
		const std::string key = SourceGroupSettings::s_keyPrefix + sourceGroupSettings->getId();
		migrator.addMigration(
			8,
			std::make_shared<SettingsMigrationMoveKey>(
				key + "/python_environment_directory_path", key + "/python_environment_path"));
	}

	return migrator;
}
