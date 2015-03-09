#include "utility/file/FileManager.h"

#include <functional>
#include <set>

#include "utility/file/FileSystem.h"

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

	for (std::map<std::string, FileInfo>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		m_removedFiles.insert(it->first);
	}

	std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> pathsExtensionsPairs;
	pathsExtensionsPairs.push_back(std::make_pair(m_includePaths, m_includeExtensions));
	pathsExtensionsPairs.push_back(std::make_pair(m_sourcePaths, m_sourceExtensions));

	for (size_t i = 0; i < pathsExtensionsPairs.size(); i++)
	{
		std::vector<FileInfo> fileInfos =
			FileSystem::getFileInfosFromDirectoryPaths(pathsExtensionsPairs[i].first, pathsExtensionsPairs[i].second);

		for (FileInfo fileInfo: fileInfos)
		{
			const std::string& filePath = fileInfo.path;
			std::map<std::string, FileInfo>::iterator it = m_files.find(filePath);
			if (it != m_files.end())
			{
				m_removedFiles.erase(filePath);
				if (fileInfo.lastWriteTime > it->second.lastWriteTime)
				{
					it->second.lastWriteTime = fileInfo.lastWriteTime;
					m_updatedFiles.insert(fileInfo.path);
				}
			}
			else
			{
				m_files.insert(std::pair<std::string, FileInfo>(filePath, fileInfo));
				m_addedFiles.insert(filePath);
			}
		}
	}

	for (const std::string filePath : m_removedFiles)
	{
		m_files.erase(filePath);
	}
}

std::set<std::string> FileManager::getAddedFilePaths() const
{
	return m_addedFiles;
}

std::set<std::string> FileManager::getUpdatedFilePaths() const
{
	return m_updatedFiles;
}

std::set<std::string> FileManager::getRemovedFilePaths() const
{
	return m_removedFiles;
}

bool FileManager::hasFilePath(const std::string& filePath) const
{
	return (m_files.find(FileSystem::absoluteFilePath(filePath)) != m_files.end());
}

bool FileManager::hasSourceExtension(const std::string& filePath) const
{
	return FileSystem::hasExtension(filePath, m_sourceExtensions);
}

bool FileManager::hasIncludeExtension(const std::string& filePath) const
{
	return FileSystem::hasExtension(filePath, m_includeExtensions);
}
