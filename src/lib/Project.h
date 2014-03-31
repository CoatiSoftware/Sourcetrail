#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "data/GraphStorage.h"
#include "data/Parser.h"
#include "ProjectSettings.h"

class Project
{
public:
	static std::shared_ptr<Project> create(
		const std::shared_ptr<CodeAccess>& codeAccess,
		const std::shared_ptr<GraphAccess>& graphAccess);

	~Project();

	const ProjectSettings& getProjectSettings() const;

private:
	Project();
	Project(const Project&);

	ProjectSettings m_settings;

	std::shared_ptr<Parser> m_parser;

	std::shared_ptr<GraphStorage> m_graphStorage;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // PROJECT_H
