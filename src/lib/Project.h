#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "data/Storage.h"
#include "ProjectSettings.h"

class Project
{
public:
	static std::shared_ptr<Project> create(
		std::shared_ptr<CodeAccess> codeAccess,
		std::shared_ptr<GraphAccess> graphAccess
	);

	~Project();

	const ProjectSettings& getProjectSettings() const;

	void parseCode();

private:
	Project();
	Project(const Project&);
	Project operator=(const Project&);

	ProjectSettings m_settings;

	std::shared_ptr<Storage> m_storage;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;
};

#endif // PROJECT_H
