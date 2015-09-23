#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "utility/file/FileManager.h"

#include "data/parser/Parser.h"
#include "data/Storage.h"

class StorageAccessProxy;

class Project
{
public:
	static std::shared_ptr<Project> create(StorageAccessProxy* storageAccessProxy);

	~Project();

	bool loadProjectSettings(const FilePath& projectSettingsFile);
	bool saveProjectSettings(const FilePath& projectSettingsFile);
	void reloadProjectSettings();

	void clearStorage();
	void parseCode();

	void logStats() const;

private:
	Project(StorageAccessProxy* storageAccessProxy);
	Project(const Project&);
	Project operator=(const Project&);

	void setProjectSettingsFilePath(const FilePath& path);
	void updateFileManager();

	Parser::Arguments getParserArguments() const;

	StorageAccessProxy* const m_storageAccessProxy;

	FilePath m_projectSettingsFilepath;
	FileManager m_fileManager;

	std::shared_ptr<Storage> m_storage;
};

#endif // PROJECT_H
