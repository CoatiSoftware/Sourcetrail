#include "Project.h"

std::shared_ptr<Project> Project::create(
	const std::shared_ptr<CodeAccess>& codeAccess,
	const std::shared_ptr<GraphAccess>& graphAccess)
{
	std::shared_ptr<Project> ptr(new Project());
	ptr->m_codeAccess = codeAccess;
	ptr->m_graphAccess = graphAccess;
	return ptr;
}

Project::Project()
{
}

Project::~Project()
{
}

const ProjectSettings& Project::getProjectSettings() const
{
	return m_settings;
}
