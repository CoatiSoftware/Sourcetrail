#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include <memory>
#include <vector>

#include "settings/migration/SettingsMigrator.h"
#include "settings/LanguageType.h"
#include "settings/Settings.h"

class SourceGroupSettings;

class ProjectSettings
	: public Settings
{
public:
	static const size_t VERSION;
	static LanguageType getLanguageOfProject(const FilePath& filePath);

	ProjectSettings();
	ProjectSettings(const FilePath& projectFilePath);
	ProjectSettings(std::string projectName, const FilePath& projectFileLocation);
	virtual ~ProjectSettings();

	bool equalsExceptNameAndLocation(const ProjectSettings& other) const;

	bool needMigration() const;
	void migrate();

	bool reload();

	std::string getProjectName() const;
	void setProjectName(const std::string& name);

	FilePath getProjectFileLocation() const;
	void setProjectFileLocation(const FilePath& location);

	std::string getDescription() const;

	std::vector<std::shared_ptr<SourceGroupSettings>> getAllSourceGroupSettings() const;
	void setAllSourceGroupSettings(std::vector<std::shared_ptr<SourceGroupSettings>> allSettings);

	std::vector<FilePath> makePathsAbsolute(const std::vector<FilePath>& paths) const;
	FilePath makePathAbsolute(const FilePath& path) const;

private:
	SettingsMigrator getMigrations() const;
};

#endif // PROJECT_SETTINGS_H
