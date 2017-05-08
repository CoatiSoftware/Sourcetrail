#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <string>
#include <set>
#include <vector>

#include "project/SourceGroup.h"

class FilePath;
class PersistentStorage;
class ProjectSettings;
class StorageAccessProxy;

class Project
{
public:
	Project(std::shared_ptr<ProjectSettings> settings, StorageAccessProxy* storageAccessProxy);
	virtual ~Project();

	bool refresh(bool forceRefresh);

	FilePath getProjectSettingsFilePath() const;
	std::string getDescription() const;

	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void setStateSettingsUpdated();

private:
	enum ProjectStateType
	{
		PROJECT_STATE_NOT_LOADED,
		PROJECT_STATE_EMPTY,
		PROJECT_STATE_LOADED,
		PROJECT_STATE_OUTDATED,
		PROJECT_STATE_OUTVERSIONED,
		PROJECT_STATE_SETTINGS_UPDATED,
		PROJECT_STATE_NEEDS_MIGRATION
	};

	Project(const Project&);

public: // todo: make private again
	void load();

private:
	bool requestIndex(bool forceRefresh, bool needsFullRefresh);

	void buildIndex(const std::set<FilePath>& filesToClean, bool fullRefresh, bool preprocessorOnly);

	bool hasCxxSourceGroup() const;

	std::shared_ptr<ProjectSettings> m_settings;
	StorageAccessProxy* const m_storageAccessProxy;

	ProjectStateType m_state;
	std::shared_ptr<PersistentStorage> m_storage;
	std::vector<std::shared_ptr<SourceGroup>> m_sourceGroups;
};

#endif // PROJECT_H
