#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <string>
#include <set>
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
	static const std::wstring PROJECT_FILE_EXTENSION;
	static const std::wstring BOOKMARK_DB_FILE_EXTENSION;
	static const std::wstring INDEX_DB_FILE_EXTENSION;
	static const std::wstring TEMP_INDEX_DB_FILE_EXTENSION;

	Project(std::shared_ptr<ProjectSettings> settings, StorageCache* storageCache, const std::string& appUUID, bool hasGUI);
	virtual ~Project();

	FilePath getProjectSettingsFilePath() const;
	std::string getDescription() const;

	bool isIndexing() const;

	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void setStateOutdated();

	void load(std::shared_ptr<DialogView> dialogView);

	void refresh(RefreshMode refreshMode, std::shared_ptr<DialogView> dialogView);

	RefreshInfo getRefreshInfo(RefreshMode mode) const;

	void buildIndex(const RefreshInfo& info, std::shared_ptr<DialogView> dialogView);

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

	void swapToTempStorage(std::shared_ptr<DialogView> dialogView);
	bool swapToTempStorageFile(const FilePath& indexDbFilePath, const FilePath& tempIndexDbFilePath, std::shared_ptr<DialogView> dialogView);
	void discardTempStorage();

	bool hasCxxSourceGroup() const;

	std::shared_ptr<ProjectSettings> m_settings;
	StorageCache* const m_storageCache;

	ProjectStateType m_state;
	bool m_isIndexing = false;

	std::shared_ptr<PersistentStorage> m_storage;
	std::vector<std::shared_ptr<SourceGroup>> m_sourceGroups;

	std::string m_appUUID;
	bool m_hasGUI;
};

#endif // PROJECT_H
