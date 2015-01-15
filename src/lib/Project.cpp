#include "Project.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>

#include "ApplicationSettings.h"
#include "data/access/GraphAccessProxy.h"
#include "data/access/LocationAccessProxy.h"
#include "data/graph/Token.h"
#include "data/parser/cxx/CxxParser.h"
#include "utility/FileSystem.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/logging/logging.h"

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
	bool success = ProjectSettings::getInstance()->load(projectSettingsFile);
	if(success)
	{
		m_projectSettingsFilepath = projectSettingsFile;
	}
	return success;
}

bool Project::saveProjectSettings( const std::string& projectSettingsFile )
{
	if(!projectSettingsFile.empty())
	{
		m_projectSettingsFilepath = projectSettingsFile;
		ProjectSettings::getInstance()->save(projectSettingsFile);
	}
	else if (!m_projectSettingsFilepath.empty())
	{
		ProjectSettings::getInstance()->save(m_projectSettingsFilepath);
	}
	else
	{
		return false;
	}
	LOG_INFO_STREAM(<< "Projectsettings saved in File: " << m_projectSettingsFilepath);
	return true;
}

void Project::clearProjectSettings()
{
	m_projectSettingsFilepath.clear();
	ProjectSettings::getInstance()->clear();
}

bool Project::setSourceDirectoryPath(const std::string& sourceDirectoryPath)
{
	m_projectSettingsFilepath = sourceDirectoryPath + "/ProjectSettings.xml";
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

		clock_t time = clock();
		parser.parseFiles(
			FileSystem::getSourceFilesFromDirectory(sourcePath, extensions),
			ApplicationSettings::getInstance()->getHeaderSearchPaths(),
			headerSearchPaths
		);
		time = clock() - time;

		// m_storage->logGraph();
		// m_storage->logLocations();

		double parseTime = (double)(time) / CLOCKS_PER_SEC;
		LOG_INFO_STREAM(<< "parse time: " << parseTime);

		MessageFinishedParsing(parseTime, m_storage->getErrorCount()).dispatch();
	}
}

Project::Project(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy)
	: m_graphAccessProxy(graphAccessProxy)
	, m_locationAccessProxy(locationAccessProxy)
{
}
