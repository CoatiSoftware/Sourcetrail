#include "Project.h"

#include <string>
#include <vector>

#include "ApplicationSettings.h"
#include "data/access/GraphAccessProxy.h"
#include "data/access/LocationAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/CxxParser.h"
#include "utility/FileSystem.h"
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
	std::string sourcePath = ProjectSettings::getInstance()->getSourcePath();
	if (sourcePath.size())
	{
		std::vector<std::string> extensions;
		extensions.push_back(".cpp");
		extensions.push_back(".cc");
		extensions.push_back(".h");
		extensions.push_back(".hpp");

		// Add the SourcePath as HeaderSearchPath as well.
		std::vector<std::string> headerSearchPaths = ProjectSettings::getInstance()->getHeaderSearchPaths();
		headerSearchPaths.push_back(sourcePath);

		CxxParser parser(m_storage.get());
		parser.parseFiles(
			FileSystem::getSourceFilesFromDirectory(sourcePath, extensions),
			ApplicationSettings::getInstance()->getHeaderSearchPaths(),
			headerSearchPaths
		);

		m_storage->logGraph();
		m_storage->logLocations();

		MessageFinishedParsing().dispatch();
	}
}

Project::Project(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy)
	: m_graphAccessProxy(graphAccessProxy)
	, m_locationAccessProxy(locationAccessProxy)
{
}
