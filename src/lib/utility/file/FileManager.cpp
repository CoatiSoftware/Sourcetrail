#include "utility/file/FileManager.h"

#include <functional>
#include <set>

#include "utility/file/FileSystem.h"
#include "ProjectSettings.h"

FileManager::FileManager(
	std::vector<std::string> sourcePaths,
	std::vector<std::string> includePaths,
	std::vector<std::string> sourceExtensions,
	std::vector<std::string> includeExtensions
)
	: m_sourcePaths(sourcePaths)
	, m_includePaths(includePaths)
	, m_sourceExtensions(sourceExtensions)
	, m_includeExtensions(includeExtensions)
{
}

FileManager::~FileManager()
{
}

void FileManager::reset()
{
	m_files.clear();
	m_addedFiles.clear();
	m_updatedFiles.clear();
	m_removedFiles.clear();
}

void FileManager::fetchFilePaths()
{
	m_addedFiles.clear();
	m_updatedFiles.clear();
	m_removedFiles.clear();

	std::set<std::string> removedFileNames;
	for (std::map<std::string, FileInfo>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		removedFileNames.insert(it->first);
	}

	std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> pathsExtensionsPairs;
	pathsExtensionsPairs.push_back(std::make_pair(m_includePaths, m_includeExtensions));
	pathsExtensionsPairs.push_back(std::make_pair(m_sourcePaths, m_sourceExtensions));

	for (size_t i = 0; i < pathsExtensionsPairs.size(); i++)
	{
		std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromDirectoryPaths(pathsExtensionsPairs[i].first, pathsExtensionsPairs[i].second);
		for (FileInfo fileInfo: fileInfos)
		{
			const std::string& filePath = fileInfo.path;
			std::map<std::string, FileInfo>::iterator it = m_files.find(filePath);
			if (it != m_files.end())
			{
				removedFileNames.erase(filePath);
				if (fileInfo.lastWriteTime > it->second.lastWriteTime)
				{
					it->second.lastWriteTime = fileInfo.lastWriteTime;
					m_updatedFiles.push_back(filePath);
				}
			}
			else
			{
				m_files.insert(std::pair<std::string, FileInfo>(filePath, fileInfo));
				m_addedFiles.push_back(filePath);
			}
		}
	}

	for (std::set<std::string>::iterator it = removedFileNames.begin(); it != removedFileNames.end(); it++)
	{
		m_files.erase(it->data());
		m_removedFiles.push_back(it->data());
	}
}

std::vector<std::string> FileManager::getAddedFilePaths() const
{
	return m_addedFiles;
}

std::vector<std::string> FileManager::getUpdatedFilePaths() const
{
	return m_updatedFiles;
}

std::vector<std::string> FileManager::getRemovedFilePaths() const
{
	return m_removedFiles;
}

bool FileManager::hasFilePath(const std::string& filePath) const
{
	return (m_files.find(filePath) != m_files.end());
}
