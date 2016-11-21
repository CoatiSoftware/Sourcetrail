#include "ApplicationStateMonitor.h"

#include "settings/ApplicationSettings.h"

std::shared_ptr<ApplicationStateMonitor> ApplicationStateMonitor::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ApplicationStateMonitor>(new ApplicationStateMonitor());
	}
	return s_instance;
}

std::vector<FilePath> ApplicationStateMonitor::getStoredIndexingFiles()
{
	return ApplicationSettings::getInstance()->getIndexingFilePaths();
}

void ApplicationStateMonitor::clearStoredIndexingFiles()
{
	ApplicationSettings::getInstance()->setIndexingFilePaths(std::vector<FilePath>());
	ApplicationSettings::getInstance()->save();
}

void ApplicationStateMonitor::clearIndexingFiles()
{
	std::lock_guard<std::mutex> lock(m_indexingFilesMutex);
	m_indexingFiles.clear();
	storeCurrentState();
}

void ApplicationStateMonitor::addIndexingFile(FilePath file)
{
	std::lock_guard<std::mutex> lock(m_indexingFilesMutex);
	m_indexingFiles.insert(file);
	storeCurrentState();
}

void ApplicationStateMonitor::removeIndexingFile(FilePath file)
{
	std::lock_guard<std::mutex> lock(m_indexingFilesMutex);
	m_indexingFiles.erase(file);
	storeCurrentState();
}

std::shared_ptr<ApplicationStateMonitor> ApplicationStateMonitor::s_instance;

ApplicationStateMonitor::ApplicationStateMonitor()
{
}

void ApplicationStateMonitor::storeCurrentState()
{
	std::vector<FilePath> indexingFiles;
	indexingFiles.insert(indexingFiles.begin(), m_indexingFiles.begin(), m_indexingFiles.end());
	ApplicationSettings::getInstance()->setIndexingFilePaths(indexingFiles);
	ApplicationSettings::getInstance()->save();
}
