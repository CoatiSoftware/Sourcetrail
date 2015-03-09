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
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/file/FileSystem.h"

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
		std::vector<std::string> includePaths;
		includePaths.push_back(sourcePath);

		// TODO: move this creation to another place (after projectsettings have been loaded)
		if (!m_fileManager)
		{
			std::vector<std::string> sourcePaths;
			sourcePaths.push_back(sourcePath);

			std::vector<std::string> sourceExtensions;
			sourceExtensions.push_back(".cpp");
			sourceExtensions.push_back(".cc");

			std::vector<std::string> includeExtensions;
			includeExtensions.push_back(".h");
			includeExtensions.push_back(".hpp");

			m_fileManager = std::make_shared<FileManager>(sourcePaths, includePaths, sourceExtensions, includeExtensions);
		}

		m_fileManager->fetchFilePaths();
		std::set<FilePath> addedFilePaths = m_fileManager->getAddedFilePaths();
		std::set<FilePath> updatedFilePaths = m_fileManager->getUpdatedFilePaths();
		std::set<FilePath> removedFilePaths = m_fileManager->getRemovedFilePaths();

		std::set<FilePath> dependingFilePaths;
		dependingFilePaths = m_storage->getDependingFilePathsAndRemoveFileNodes(updatedFilePaths);
		updatedFilePaths.insert(dependingFilePaths.begin(), dependingFilePaths.end());

		dependingFilePaths = m_storage->getDependingFilePathsAndRemoveFileNodes(removedFilePaths);
		updatedFilePaths.insert(dependingFilePaths.begin(), dependingFilePaths.end());

		m_storage->clearFileData(updatedFilePaths);
		m_storage->clearFileData(removedFilePaths);

		std::vector<FilePath> filesToParse;
		filesToParse.insert(filesToParse.end(), addedFilePaths.begin(), addedFilePaths.end());
		filesToParse.insert(filesToParse.end(), updatedFilePaths.begin(), updatedFilePaths.end());

		if (filesToParse.size() == 0)
		{
			MessageFinishedParsing(0, 0, m_storage->getErrorCount()).dispatch();
		}
		else
		{
			// Add the SourcePaths as HeaderSearchPaths as well, so clang will also look here when searching include files.
			std::vector<std::string> headerSearchPaths = ProjectSettings::getInstance()->getHeaderSearchPaths();
			for (size_t i = 0; i < includePaths.size(); i++)
			{
				headerSearchPaths.push_back(includePaths[i]);
			}

			// std::cout << "parse files" << std::endl;
			// for (const FilePath& path : filesToParse)
			// {
			// 	std::cout << path.absoluteStr() << std::endl;
			// }
			// std::cout << std::endl;

			CxxParser parser(m_storage.get(), m_fileManager.get());
			clock_t time = clock();
			parser.parseFiles(
				filesToParse,
				ApplicationSettings::getInstance()->getHeaderSearchPaths(),
				headerSearchPaths
			);
			time = clock() - time;

			// m_storage->logGraph();
			// m_storage->logLocations();

			double parseTime = (double)(time) / CLOCKS_PER_SEC;
			LOG_INFO_STREAM(<< "parse time: " << parseTime);

			MessageFinishedParsing(filesToParse.size(), parseTime, m_storage->getErrorCount()).dispatch();
		}
	}
}

Project::Project(GraphAccessProxy* graphAccessProxy, LocationAccessProxy* locationAccessProxy)
	: m_graphAccessProxy(graphAccessProxy)
	, m_locationAccessProxy(locationAccessProxy)
{
}
