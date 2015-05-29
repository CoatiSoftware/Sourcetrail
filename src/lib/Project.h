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

	bool loadProjectSettings(const std::string& projectSettingsFile);
	bool saveProjectSettings(const std::string& projectSettingsFile);
	void clearProjectSettings();

	bool setSourceDirectoryPath(const std::string& sourceDirectoryPath);

	void clearStorage();
	void parseCode();

	void logStats() const;

private:
	Project(StorageAccessProxy* storageAccessProxy);
	Project(const Project&);
	Project operator=(const Project&);

	void createFileManager();

	Parser::Arguments getParserArguments() const;

	std::string m_projectSettingsFilepath;

	StorageAccessProxy* const m_storageAccessProxy;

	std::shared_ptr<Storage> m_storage;
	std::shared_ptr<FileManager> m_fileManager;
};

#endif // PROJECT_H
