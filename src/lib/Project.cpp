#include "Project.h"

#include <string>
#include <vector>

#include "data/parser/cxx/CxxParser.h"
#include "utility/FileSystem.h"
#include "utility/logging/logging.h"

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

bool Project::loadProjectSettings(const std::string& projectSettingsFile)
{
	return ProjectSettings::getInstance()->load(projectSettingsFile);
}

void Project::parseCode()
{
	if (ProjectSettings::getInstance()->getSourcePath() != "")
	{
		std::vector<std::string> extension;
		extension.push_back(".cpp");
		extension.push_back(".h");

		CxxParser parser(m_storage);
		parser.parseFiles(
			FileSystem::getSourceFilesFromDirectory(ProjectSettings::getInstance()->getSourcePath(), extension)
		);

		m_storage->logGraph();
		m_storage->logLocations();
	}
}
