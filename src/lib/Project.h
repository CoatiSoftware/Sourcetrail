#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "data/Storage.h"
#include "utility/file/FileManager.h"
#include "ProjectSettings.h"

class GraphAccessProxy;
class LocationAccessProxy;

class Project
{
public:
	static std::shared_ptr<Project> create(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy);

	~Project();

	bool loadProjectSettings(const std::string& projectSettingsFile);
	bool saveProjectSettings(const std::string& projectSettingsFile);
	void clearProjectSettings();

	bool setSourceDirectoryPath(const std::string& sourceDirectoryPath);

	void clearStorage();
	void parseCode();

private:
	Project(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy);
	Project(const Project&);
	Project operator=(const Project&);

	std::string m_projectSettingsFilepath;

	GraphAccessProxy* const m_graphAccessProxy;
	LocationAccessProxy* const m_locationAccessProxy;

	std::shared_ptr<Storage> m_storage;
	std::shared_ptr<FileManager> m_fileManager;
};

#endif // PROJECT_H
