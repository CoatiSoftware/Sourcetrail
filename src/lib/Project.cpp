#include "Project.h"

#include <string>
#include <vector>

#include "data/parser/cxx/CxxParser.h"
#include "utility/FileSystem.h"
#include "utility/messaging/type/MessageActivateTokenLocation.h"
#include "utility/logging/logging.h"

std::shared_ptr<Project> Project::create(
	std::shared_ptr<Storage> storage
)
{
	std::shared_ptr<Project> ptr(new Project());
	ptr->m_storage = storage;
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
		std::vector<std::string> extensions;
		extensions.push_back(".cpp");
		extensions.push_back(".h");

		CxxParser parser(m_storage);
		parser.parseFiles(
			FileSystem::getSourceFilesFromDirectory(ProjectSettings::getInstance()->getSourcePath(), extensions)
		);

		m_storage->logGraph();
		m_storage->logLocations();

		MessageActivateTokenLocation message;
		message.dispatch();
	}
}
