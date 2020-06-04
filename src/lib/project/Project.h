#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "RefreshInfo.h"
#include "SourceGroup.h"

struct FileInfo;
class DialogView;
class FilePath;
class PersistentStorage;
class ProjectSettings;
class StorageCache;

class Project
{
public:
	Project(
		std::shared_ptr<ProjectSettings> settings,
		StorageCache* storageCache,
		const std::string& appUUID,
		bool hasGUI);
	virtual ~Project();

	FilePath getProjectSettingsFilePath() const;
	std::string getDescription() const;

	bool isLoaded() const;
	bool isIndexing() const;

	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void setStateOutdated();

	void load(std::shared_ptr<DialogView> dialogView);

	void refresh(
		std::shared_ptr<DialogView> dialogView, RefreshMode refreshMode, bool shallowIndexingRequested);

	RefreshInfo getRefreshInfo(RefreshMode mode) const;

	void buildIndex(RefreshInfo info, std::shared_ptr<DialogView> dialogView);

private:
	enum ProjectStateType
	{
		PROJECT_STATE_NOT_LOADED,
		PROJECT_STATE_EMPTY,
		PROJECT_STATE_LOADED,
		PROJECT_STATE_OUTDATED,
		PROJECT_STATE_OUTVERSIONED,
		PROJECT_STATE_NEEDS_MIGRATION,
		PROJECT_STATE_DB_CORRUPTED
	};

	enum class RefreshStageType
	{
		REFRESHING,
		INDEXING,
		NONE
	};

	Project(const Project&);

	void swapToTempStorage(std::shared_ptr<DialogView> dialogView);
	bool swapToTempStorageFile(
		const FilePath& indexDbFilePath,
		const FilePath& tempIndexDbFilePath,
		std::shared_ptr<DialogView> dialogView);
	void discardTempStorage();

	bool hasCxxSourceGroup() const;

	std::shared_ptr<ProjectSettings> m_settings;
	StorageCache* const m_storageCache;

	ProjectStateType m_state;
	RefreshStageType m_refreshStage;

	std::shared_ptr<PersistentStorage> m_storage;
	std::vector<std::shared_ptr<SourceGroup>> m_sourceGroups;

	std::string m_appUUID;
	bool m_hasGUI;
};

#endif	  // PROJECT_H
