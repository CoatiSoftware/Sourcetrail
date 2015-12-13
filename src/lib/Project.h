#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "utility/file/FileManager.h"

#include "data/parser/Parser.h"

class Storage;
class StorageAccessProxy;

class Project
{
public:
	static std::shared_ptr<Project> create(StorageAccessProxy* storageAccessProxy);

	~Project();

	bool loadProject(const FilePath& projectSettingsFile);
	bool saveProject(const FilePath& projectSettingsFile);
	void reloadProject();

	void clearStorage();

	void logStats() const;

private:
	Project(StorageAccessProxy* storageAccessProxy);
	Project(const Project&);
	Project operator=(const Project&);

	void parseCode();

	void setProjectSettingsFilePath(const FilePath& path);
	void updateFileManager();

	Parser::Arguments getParserArguments() const;

	StorageAccessProxy* const m_storageAccessProxy;

	FilePath m_projectSettingsFilepath;
	FileManager m_fileManager;

	std::shared_ptr<Storage> m_storage;
	bool m_storageWasLoaded;
};

#endif // PROJECT_H
