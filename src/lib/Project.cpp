#include "Project.h"

#include <vector>
#include <string>

#include "data/parser/cxx/CxxParser.h"

std::shared_ptr<Project> Project::create(
	std::shared_ptr<CodeAccess> codeAccess,
	std::shared_ptr<GraphAccess> graphAccess
)
{
	std::shared_ptr<Project> ptr(new Project());
	ptr->m_codeAccess = codeAccess;
	ptr->m_graphAccess = graphAccess;

	ptr->m_storage = std::make_shared<Storage>();

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

void Project::parseCode()
{
	std::vector<std::string> filePaths;
	filePaths.push_back("data/test_code.cpp");

	CxxParser parser(m_storage);
	parser.parseFiles(filePaths);
}
