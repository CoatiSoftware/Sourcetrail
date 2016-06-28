#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "utility/file/FileManager.h"

#include "data/parser/Parser.h"

class PersistentStorage;
class StorageAccessProxy;

class Project
{
public:
	enum ProjectState
	{
		PROJECT_NONE,
		PROJECT_EMPTY,
		PROJECT_LOADED,
		PROJECT_OUTDATED,
		PROJECT_OUTVERSIONED
	};

	static std::shared_ptr<Project> create(StorageAccessProxy* storageAccessProxy);

	~Project();

	const FilePath& getProjectSettingsFilePath() const;

	ProjectState load(const FilePath& projectSettingsFile);
	ProjectState reload();

	void clearStorage();

	void logStats() const;

private:
	Project(StorageAccessProxy* storageAccessProxy);
	Project(const Project&);
	Project operator=(const Project&);

	void parseCode();

	void setProjectSettingsFilePath(const FilePath& path);
	void loadStorage(const FilePath& path);
	void updateFileManager();

	Parser::Arguments getParserArguments() const;

	StorageAccessProxy* const m_storageAccessProxy;

	ProjectState m_state;

	FilePath m_projectSettingsFilepath;
	FileManager m_fileManager;

	std::shared_ptr<PersistentStorage> m_storage;
};

#endif // PROJECT_H
