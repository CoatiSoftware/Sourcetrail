#include "Project.h"

#include <string>
#include <vector>

#include "data/access/GraphAccessProxy.h"
#include "data/access/LocationAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/CxxParser.h"
#include "utility/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

std::shared_ptr<Project> Project::create(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy)
{
	std::shared_ptr<Project> ptr(new Project(graphAccessProxy, locationAccessProxy));
	ptr->clearStorage();
	return ptr;
}

Project::~Project()
{
}

bool Project::loadProjectSettings(const std::string& projectSettingsFile)
{
	return ProjectSettings::getInstance()->load(projectSettingsFile);
}

void Project::clearProjectSettings()
{
	ProjectSettings::getInstance()->clear();
}

bool Project::setSourceDirectoryPath(const std::string& sourceDirectoryPath)
{
	return ProjectSettings::getInstance()->setSourcePath(sourceDirectoryPath);
}

void Project::clearStorage()
{
	m_storage = std::make_shared<Storage>();

	m_graphAccessProxy->setSubject(m_storage.get());
	m_locationAccessProxy->setSubject(m_storage.get());

	Token::resetNextId();
}

void Project::parseCode()
{
	if (ProjectSettings::getInstance()->getSourcePath() != "")
	{
		std::vector<std::string> extensions;
		extensions.push_back(".cpp");
		extensions.push_back(".cc");
		extensions.push_back(".h");
		extensions.push_back(".hpp");

		CxxParser parser(m_storage);
		parser.parseFiles(
			FileSystem::getSourceFilesFromDirectory(ProjectSettings::getInstance()->getSourcePath(), extensions)
		);

		m_storage->logGraph();
		m_storage->logLocations();

		MessageFinishedParsing message;
		message.dispatch();
	}
}

Project::Project(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy)
	: m_graphAccessProxy(graphAccessProxy)
	, m_locationAccessProxy(locationAccessProxy)
{
}
