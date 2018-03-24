#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <string>
#include <set>
#include <vector>

#include "project/RefreshInfo.h"
#include "project/SourceGroup.h"

struct FileInfo;
class DialogView;
class FilePath;
class PersistentStorage;
class ProjectSettings;
class StorageCache;

class Project
{
public:
	Project(std::shared_ptr<ProjectSettings> settings, StorageCache* storageCache, bool hasGUI);
	virtual ~Project();

	FilePath getProjectSettingsFilePath() const;
	std::string getDescription() const;

	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void setStateOutdated();

	void load();

	void refresh(RefreshMode refreshMode, DialogView* dialogView);

	RefreshInfo getRefreshInfo(RefreshMode mode) const;

	void buildIndex(const RefreshInfo& info, DialogView* dialogView);

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

	Project(const Project&);

	std::set<FilePath> getAllSourceFilePaths() const;

	RefreshInfo getRefreshInfoForUpdatedFiles() const;
	RefreshInfo getRefreshInfoForIncompleteFiles() const;
	RefreshInfo getRefreshInfoForAllFiles() const;

	bool hasCxxSourceGroup() const;
	bool didFileChange(const FileInfo& info) const;

	std::shared_ptr<ProjectSettings> m_settings;
	StorageCache* const m_storageCache;

	ProjectStateType m_state;
	std::shared_ptr<PersistentStorage> m_storage;
	std::vector<std::shared_ptr<SourceGroup>> m_sourceGroups;

	bool m_hasGUI;
};

#endif // PROJECT_H
