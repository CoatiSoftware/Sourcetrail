#ifndef PROJECT_H
#define PROJECT_H

#include <memory>
#include <mutex>
#include <set>

#include "utility/file/FileManager.h"

#include "data/parser/Parser.h"
#include "settings/LanguageType.h"

class DialogView;
class FileRegister;
class PersistentStorage;
class StorageProvider;
class ProjectSettings;
class StorageAccessProxy;
class IndexerCommand;

class Project
{
public:
	virtual ~Project();

	bool refresh(bool forceRefresh);

	FilePath getProjectSettingsFilePath() const;
	LanguageType getLanguage() const;
	std::string getDescription() const;

	std::set<FilePath> getSourceFilePaths() const;

	bool settingsEqualExceptNameAndLocation(const ProjectSettings& otherSettings) const;
	void setStateSettingsUpdated();

protected:
	Project(StorageAccessProxy* storageAccessProxy, DialogView* dialogView);
	DialogView* getDialogView() const;
	std::vector<FilePath> getSourcePaths() const;

	virtual std::shared_ptr<ProjectSettings> getProjectSettings() = 0;
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const = 0;

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
	void buildIndex(const std::set<FilePath>& filesToClean, const std::set<FilePath>& filesToIndex, bool fullRefresh);

	virtual bool prepareIndexing();
	virtual bool prepareRefresh();
	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands() = 0;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& sourceFiles);
	virtual void updateFileManager(FileManager& fileManager) = 0;

	StorageAccessProxy* const m_storageAccessProxy;
	DialogView* m_dialogView;

	ProjectStateType m_state;
	FileManager m_fileManager;

	std::shared_ptr<PersistentStorage> m_storage;
};

#endif // PROJECT_H
