#include "settings/ProjectSettings.h"

#include "settings/migration/SettingsMigrationDeleteKey.h"
#include "settings/migration/SettingsMigrationLambda.h"
#include "settings/migration/SettingsMigrationMoveKey.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/utilityUuid.h"

const size_t ProjectSettings::VERSION = 4;
const std::string PROJECT_FILE_EXTENSION = ".srctrlprj";

LanguageType ProjectSettings::getLanguageOfProject(const FilePath& filePath)
{
	LanguageType languageType = LANGUAGE_UNKNOWN;

	ProjectSettings projectSettings;
	projectSettings.load(filePath);
	for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: projectSettings.getAllSourceGroupSettings())
	{
		const LanguageType currentLanguageType = getLanguageTypeForSourceGroupType(sourceGroupSettings->getType());
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

ProjectSettings::ProjectSettings()
{
}

ProjectSettings::ProjectSettings(const FilePath& projectFilePath)
{
	setFilePath(projectFilePath);
}

ProjectSettings::ProjectSettings(std::string projectName, const FilePath& projectFileLocation)
{
	setFilePath(FilePath(projectFileLocation.str() + "/" + projectName + PROJECT_FILE_EXTENSION));
}

ProjectSettings::~ProjectSettings()
{
}

bool ProjectSettings::equalsExceptNameAndLocation(const ProjectSettings& other) const
{
	const std::vector<std::shared_ptr<SourceGroupSettings>> allMySettings = getAllSourceGroupSettings();
	const std::vector<std::shared_ptr<SourceGroupSettings>> allOtherSettings = other.getAllSourceGroupSettings();

	if (allMySettings.size() != allOtherSettings.size())
	{
		return false;
	}

	for (std::shared_ptr<SourceGroupSettings> mySourceGroup: allMySettings)
	{
		bool matched = false;
		for (std::shared_ptr<SourceGroupSettings> otherSourceGroup: allOtherSettings)
		{
			if (mySourceGroup->equals(otherSourceGroup))
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

std::string ProjectSettings::getProjectName() const
{
	return getFilePath().withoutExtension().fileName();
}

void ProjectSettings::setProjectName(const std::string& name)
{
	setFilePath(FilePath(getProjectFileLocation().str() + "/" + name + PROJECT_FILE_EXTENSION));
}

FilePath ProjectSettings::getProjectFileLocation() const
{
	return getFilePath().parentDirectory();
}

void ProjectSettings::setProjectFileLocation(const FilePath& location)
{
	setFilePath(FilePath(location.str() + "/" + getProjectName() + PROJECT_FILE_EXTENSION));
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
		const std::string id = key.substr(std::string("source_groups/source_group_").length());
		const SourceGroupType type = stringToSourceGroupType(getValue<std::string>(key + "/type", ""));

		std::shared_ptr<SourceGroupSettings> settings;

		switch (type)
		{
		case SOURCE_GROUP_C_EMPTY:
		case SOURCE_GROUP_CPP_EMPTY:
		case SOURCE_GROUP_CXX_CDB:
			{
				std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::make_shared<SourceGroupSettingsCxx>(id, type, this);
				cxxSettings->setHeaderSearchPaths(getPathValues(key + "/header_search_paths/header_search_path"));
				cxxSettings->setFrameworkSearchPaths(getPathValues(key + "/framework_search_paths/framework_search_path"));
				cxxSettings->setCompilerFlags(getValues<std::string>(key + "/compiler_flags/compiler_flag", std::vector<std::string>()));
				cxxSettings->setUseSourcePathsForHeaderSearch(getValue<bool>(key + "/use_source_paths_for_header_search", false));
				cxxSettings->setHasDefinedUseSourcePathsForHeaderSearch(isValueDefined(key + "/use_source_paths_for_header_search"));
				if (type == SOURCE_GROUP_CXX_CDB)
				{
					cxxSettings->setCompilationDatabasePath(FilePath(getValue<std::string>(key + "/build_file_path/compilation_db_path", "")));
				}
				settings = cxxSettings;
			}
		break;
		case SOURCE_GROUP_JAVA_EMPTY:
		case SOURCE_GROUP_JAVA_MAVEN:
			{
				std::shared_ptr<SourceGroupSettingsJava> javaSettings = std::make_shared<SourceGroupSettingsJava>(id, type, this);
				javaSettings->setClasspaths(getPathValues(key + "/class_paths/class_path"));
				if (type == SOURCE_GROUP_JAVA_MAVEN)
				{
					javaSettings->setMavenProjectFilePath(FilePath(getValue<std::string>(key + "/maven/project_file_path", "")));
					javaSettings->setMavenDependenciesDirectory(FilePath(getValue<std::string>(key + "/maven/dependencies_directory", "")));
					javaSettings->setShouldIndexMavenTests(getValue<bool>(key + "/maven/should_index_tests", false));
				}
				settings = javaSettings;
			}
			break;
		default:
			continue;
		}

		settings->setStandard(getValue<std::string>(key + "/standard", ""));
		settings->setSourcePaths(getPathValues(key + "/source_paths/source_path"));
		settings->setExcludePaths(getPathValues(key + "/exclude_paths/exclude_path"));
		settings->setSourceExtensions(getValues(key + "/source_extensions/source_extension", std::vector<std::string>()));

		allSettings.push_back(settings);
	}

	return allSettings;
}

void ProjectSettings::setAllSourceGroupSettings(std::vector<std::shared_ptr<SourceGroupSettings>> allSettings)
{
	for (const std::string& key: m_config->getSublevelKeys("source_groups"))
	{
		m_config->removeValues(key);
	}

	for (std::shared_ptr<SourceGroupSettings> settings: allSettings)
	{
		const std::string key = "source_groups/source_group_" + settings->getId();
		const SourceGroupType type = settings->getType();

		setValue(key + "/type", sourceGroupTypeToString(type));
		setValue(key + "/standard", settings->getStandard());
		setPathValues(key + "/source_paths/source_path", settings->getSourcePaths());
		setPathValues(key + "/exclude_paths/exclude_path", settings->getExcludePaths());
		setValues(key + "/source_extensions/source_extension", settings->getSourceExtensions());

		switch (type)
		{
		case SOURCE_GROUP_C_EMPTY:
		case SOURCE_GROUP_CPP_EMPTY:
		case SOURCE_GROUP_CXX_CDB:
			if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(settings))
			{
				setPathValues(key + "/header_search_paths/header_search_path", cxxSettings->getHeaderSearchPaths());
				setPathValues(key + "/framework_search_paths/framework_search_path", cxxSettings->getFrameworkSearchPaths());
				setValues(key + "/compiler_flags/compiler_flag", cxxSettings->getCompilerFlags());
				if (cxxSettings->getHasDefinedUseSourcePathsForHeaderSearch())
				{
					setValue(key + "/use_source_paths_for_header_search", cxxSettings->getUseSourcePathsForHeaderSearch());
				}
				if (type == SOURCE_GROUP_CXX_CDB)
				{
					setValue(key + "/build_file_path/compilation_db_path", cxxSettings->getCompilationDatabasePath().str());
				}
			}
		break;
		case SOURCE_GROUP_JAVA_EMPTY:
		case SOURCE_GROUP_JAVA_MAVEN:
			if (std::shared_ptr<SourceGroupSettingsJava> javaSettings = std::dynamic_pointer_cast<SourceGroupSettingsJava>(settings))
			{
				setPathValues(key + "/class_paths/class_path", javaSettings->getClasspaths());

				if (type == SOURCE_GROUP_JAVA_MAVEN)
				{
					setValue(key + "/maven/project_file_path", javaSettings->getMavenProjectFilePath().str());
					setValue(key + "/maven/dependencies_directory", javaSettings->getMavenDependenciesDirectory().str());
					setValue(key + "/maven/should_index_tests", javaSettings->getShouldIndexMavenTests());
				}
			}
		break;
		default:
			continue;
		}
	}
}

std::vector<FilePath> ProjectSettings::makePathsAbsolute(const std::vector<FilePath>& paths) const
{
	std::vector<FilePath> absPaths;

	FilePath basePath = getProjectFileLocation();
	for (const FilePath& path : paths)
	{
		if (path.isAbsolute())
		{
			absPaths.push_back(path);
		}
		else
		{
			absPaths.push_back(basePath.concat(path).canonical());
		}
	}

	return absPaths;
}

FilePath ProjectSettings::makePathAbsolute(const FilePath& path) const
{
	if (path.empty() || path.isAbsolute())
	{
		return path;
	}

	return getProjectFileLocation().concat(path).canonical();
}

SettingsMigrator ProjectSettings::getMigrations() const
{
	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationLambda>(
		[](const SettingsMigration* migration, Settings* settings)
		{
			const std::string language = migration->getValueFromSettings<std::string>(settings, "language_settings/language", "");
			const std::string standard = migration->getValueFromSettings<std::string>(settings, "language_settings/standard", "");

			if (language == "C" && !utility::isPrefix("c", standard))
			{
				migration->setValueInSettings(settings, "language_settings/standard", "c" + standard);
			}

			if (language == "C++" && !utility::isPrefix("c++", standard))
			{
				migration->setValueInSettings(settings, "language_settings/standard", "c++" + standard);
			}
		}
	));

	const std::string sourceGroupKey = "source_groups/source_group_" + utility::getUuidString();

	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("info/description", "description"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("language_settings/standard", sourceGroupKey + "/standard"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/source_paths/source_path", sourceGroupKey + "/source_paths/source_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/exclude_paths/exclude_path", sourceGroupKey + "/exclude_paths/exclude_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/extensions/source_extensions", sourceGroupKey + "/source_extensions/source_extension"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/header_search_paths/header_search_path", sourceGroupKey + "/header_search_paths/header_search_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/use_source_paths_for_header_search", sourceGroupKey + "/use_source_paths_for_header_search"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/framework_search_paths/framework_search_path", sourceGroupKey + "/framework_search_paths/framework_search_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/compiler_flags/compiler_flag", sourceGroupKey + "/compiler_flags/compiler_flag"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/build_file_path/compilation_db_path", sourceGroupKey + "/build_file_path/compilation_db_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/class_paths/class_path", sourceGroupKey + "/class_paths/class_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/maven/project_file_path", sourceGroupKey + "/maven/project_file_path"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/maven/dependencies_directory", sourceGroupKey + "/maven/dependencies_directory"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("source/maven/should_index_tests", sourceGroupKey + "/maven/should_index_tests"));

	migrator.addMigration(3, std::make_shared<SettingsMigrationLambda>(
		[=](const SettingsMigration* migration, Settings* settings)
		{
			const std::string language = migration->getValueFromSettings<std::string>(settings, "language_settings/language", "");

			SourceGroupType type = SOURCE_GROUP_UNKNOWN;
			if (language == "C" || language == "C++")
			{
				const std::string cdbPath = migration->getValueFromSettings<std::string>(settings, sourceGroupKey + "/build_file_path/compilation_db_path", "");
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
			else if (language == "Java")
			{
				const std::string mavenProjectFilePath = migration->getValueFromSettings<std::string>(settings, sourceGroupKey + "/maven/project_file_path", "");
				if (!mavenProjectFilePath.empty())
				{
					type = SOURCE_GROUP_JAVA_MAVEN;
				}
				else
				{
					type = SOURCE_GROUP_JAVA_EMPTY;
				}
			}

			migration->setValueInSettings(settings, sourceGroupKey + "/type", sourceGroupTypeToString(type));
		}
	));

	migrator.addMigration(4, std::make_shared<SettingsMigrationDeleteKey>("language_settings/language"));
	migrator.addMigration(4, std::make_shared<SettingsMigrationDeleteKey>("source/build_file_path/vs_solution_path"));
	migrator.addMigration(4, std::make_shared<SettingsMigrationDeleteKey>("source/extensions/header_extensions"));

	return migrator;
}


