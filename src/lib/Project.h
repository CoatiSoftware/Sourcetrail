#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <mutex>

#include "utility/file/FileManager.h"

#include "data/parser/Parser.h"
#include "settings/ProjectSettings.h" // todo: use forward declaration here
#include "utility/scheduling/Task.h"

class PersistentStorage;
class StorageAccessProxy;
class FileRegister;

class Project
{
public:
	static std::shared_ptr<Project> create(const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy);

	virtual ~Project();

	void refresh();
	void forceRefresh();

	FilePath getProjectSettingsFilePath() const;
	LanguageType getLanguage() const;
	std::string getDescription() const;
	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void logStats() const;

protected:
	Project(StorageAccessProxy* storageAccessProxy);

	virtual std::shared_ptr<ProjectSettings> getProjectSettings() = 0;
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const = 0;

private:
	enum ProjectStateType
	{
		PROJECT_STATE_NOT_LOADED,
		PROJECT_STATE_EMPTY,
		PROJECT_STATE_LOADED,
		PROJECT_STATE_OUTDATED,
		PROJECT_STATE_OUTVERSIONED
	};

	Project(const Project&);

	void load();
	void clearStorage();
	void buildIndex();

	virtual bool allowsRefresh();
	virtual std::shared_ptr<Task> createIndexerTask(
		PersistentStorage* storage,
		std::shared_ptr<std::mutex> storageMutex,
		std::shared_ptr<FileRegister> fileRegister) = 0;
	virtual void updateFileManager(FileManager& fileManager) = 0;

	StorageAccessProxy* const m_storageAccessProxy;

	ProjectStateType m_state;
	FileManager m_fileManager;

	std::shared_ptr<PersistentStorage> m_storage;
};

#endif // PROJECT_H
