#include "settings/ProjectSettings.h"

#include "settings/CxxProjectSettings.h"
#include "settings/JavaProjectSettings.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

const size_t ProjectSettings::VERSION = 1;

LanguageType ProjectSettings::getLanguageOfProject(FilePath projectFilePath)
{
	ProjectSettings tempSettings;
	tempSettings.load(projectFilePath);
	return tempSettings.getLanguage();
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
	setFilePath(FilePath(projectFileLocation.str() + "/" + projectName + ".coatiproject"));
}

ProjectSettings::~ProjectSettings()
{
}

bool ProjectSettings::equalsExceptNameAndLocation(const ProjectSettings& other) const
{
	return (
		getLanguage() == other.getLanguage() &&
		getStandard() == other.getStandard() &&
		utility::isPermutation<FilePath>(getSourcePaths(), other.getSourcePaths()) &&
		utility::isPermutation<FilePath>(getExcludePaths(), other.getExcludePaths()) &&
		utility::isPermutation<std::string>(getSourceExtensions(), other.getSourceExtensions())
	);
}

std::vector<std::string> ProjectSettings::getLanguageStandards() const
{
	return std::vector<std::string>();
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
	setFilePath(FilePath(getProjectFileLocation().str() + "/" + name + ".coatiproject"));
}

FilePath ProjectSettings::getProjectFileLocation() const
{
	return getFilePath().parentDirectory();
}

void ProjectSettings::setProjectFileLocation(const FilePath& location)
{
	setFilePath(FilePath(location.str() + "/" + getProjectName() + ".coatiproject"));
}

std::string ProjectSettings::getDescription() const
{
	return getValue<std::string>("info/description", "");
}

LanguageType ProjectSettings::getLanguage() const
{
	return stringToLanguageType(getValue<std::string>("language_settings/language", languageTypeToString(LANGUAGE_UNKNOWN)));
}

bool ProjectSettings::setLanguage(LanguageType language)
{
	return setValue<std::string>("language_settings/language", languageTypeToString(language));
}

std::string ProjectSettings::getStandard() const
{
	return getValue<std::string>("language_settings/standard", getDefaultStandard());
}

bool ProjectSettings::setStandard(const std::string& standard)
{
	return setValue<std::string>("language_settings/standard", standard);
}

std::vector<FilePath> ProjectSettings::getSourcePaths() const // TODO: rename to getIndexedPaths
{
	return getPathValues("source/source_paths/source_path");
}

std::vector<FilePath> ProjectSettings::getAbsoluteSourcePaths() const
{
	return makePathsAbsolute(expandPaths(getSourcePaths()));
}

bool ProjectSettings::setSourcePaths(const std::vector<FilePath>& sourcePaths)
{
	return setPathValues("source/source_paths/source_path", sourcePaths);
}

std::vector<FilePath> ProjectSettings::getExcludePaths() const
{
	return getPathValues("source/exclude_paths/exclude_path");
}

std::vector<FilePath> ProjectSettings::getAbsoluteExcludePaths() const
{
	return makePathsAbsolute(expandPaths(getExcludePaths()));
}

bool ProjectSettings::setExcludePaths(const std::vector<FilePath>& excludePaths)
{
	return setPathValues("source/exclude_paths/exclude_path", excludePaths);
}

std::vector<std::string> ProjectSettings::getSourceExtensions() const
{
	return getValues("source/extensions/source_extensions", getDefaultSourceExtensions());
}

bool ProjectSettings::setSourceExtensions(const std::vector<std::string> &sourceExtensions)
{
	return setValues("source/extensions/source_extensions", sourceExtensions);
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

std::vector<std::string> ProjectSettings::getDefaultSourceExtensions() const
{
	return std::vector<std::string>();
}

std::string ProjectSettings::getDefaultStandard() const
{
	return "";
}

SettingsMigrator ProjectSettings::getMigrations() const
{
	SettingsMigrator migrator;

	if (getLanguage() == LANGUAGE_C || getLanguage() == LANGUAGE_CPP)
	{
		migrator.addLambdaMigration(1,
			[](Settings* settings)
			{
				ProjectSettings* s = dynamic_cast<ProjectSettings*>(settings);

				if (s->getLanguage() == LANGUAGE_C && !utility::isPrefix("c", s->getStandard()))
				{
					s->setStandard("c" + s->getStandard());
				}

				if (s->getLanguage() == LANGUAGE_CPP && !utility::isPrefix("c++", s->getStandard()))
				{
					s->setStandard("c++" + s->getStandard());
				}
			}
		);
	}

	return migrator;
}


