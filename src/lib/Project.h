#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "data/Storage.h"
#include "ProjectSettings.h"

class Project
{
public:
	static std::shared_ptr<Project> create(
		std::shared_ptr<Storage> storage
	);

	~Project();

	bool loadProjectSettings(const std::string& projectSettingsFile);
	void clearProjectSettings();

	bool setSourceDirectoryPath(const std::string& sourceDirectoryPath);

	void parseCode();

private:
	Project();
	Project(const Project&);
	Project operator=(const Project&);

	std::shared_ptr<Storage> m_storage;
};

#endif // PROJECT_H
