#include "utility/file/FileManager.h"

#include <functional>
#include <set>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

const std::vector<FilePath>& FileManager::getSourcePaths() const
{
	return m_sourcePaths;
}

const std::vector<FilePath>& FileManager::getIncludePaths() const
{
	return m_includePaths;
}

void FileManager::setPaths(
	std::vector<FilePath> sourcePaths,
	std::vector<FilePath> includePaths,
	std::vector<std::string> sourceExtensions,
	std::vector<std::string> includeExtensions
){
	m_sourcePaths = sourcePaths;
	m_includePaths = includePaths;
	m_sourceExtensions = sourceExtensions;
	m_includeExtensions = includeExtensions;
}

void FileManager::clear()
{
	m_files.clear();
	m_addedFiles.clear();
	m_updatedFiles.clear();
	m_removedFiles.clear();
}

void FileManager::fetchFilePaths(const std::vector<FileInfo>& oldFileInfos)
{
	for (FileInfo oldFileInfo: oldFileInfos)
	{
		m_files[oldFileInfo.path] = oldFileInfo;
	}

	m_addedFiles.clear();
	m_updatedFiles.clear();
	m_removedFiles.clear();

	for (std::map<FilePath, FileInfo>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		m_removedFiles.insert(it->first);
	}

	std::vector<FileInfo> fileInfos = getFileInfosInProject();
	for (FileInfo fileInfo: fileInfos)
	{
		const FilePath& filePath = fileInfo.path;
		std::map<FilePath, FileInfo>::iterator it = m_files.find(filePath);
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
			m_files.insert(std::pair<FilePath, FileInfo>(filePath, fileInfo));
			m_addedFiles.insert(filePath);
		}
	}

	for (const FilePath& filePath : m_removedFiles)
	{
		m_files.erase(filePath);
	}
}

std::set<FilePath> FileManager::getAddedFilePaths() const
{
	return m_addedFiles;
}

std::set<FilePath> FileManager::getUpdatedFilePaths() const
{
	return m_updatedFiles;
}

std::set<FilePath> FileManager::getRemovedFilePaths() const
{
	return m_removedFiles;
}

bool FileManager::hasFilePath(const FilePath& filePath) const
{
	return (m_files.find(filePath) != m_files.end());
}

bool FileManager::hasSourceExtension(const FilePath& filePath) const
{
	return filePath.hasExtension(m_sourceExtensions);
}

bool FileManager::hasIncludeExtension(const FilePath& filePath) const
{
	return filePath.hasExtension(m_includeExtensions);
}

const FileInfo& FileManager::getFileInfo(const FilePath& filePath) const
{
	std::map<FilePath, FileInfo>::const_iterator it = m_files.find(filePath);

	if (it == m_files.end())
	{
		LOG_ERROR("No FileInfo found for file: " + filePath.str());
	}

	return it->second;
}

std::vector<FileInfo> FileManager::getFileInfosInProject() const
{
	std::vector<FileInfo> fileInfos;

	std::vector<std::pair<std::vector<FilePath>, std::vector<std::string>>> pathsExtensionsPairs;
	pathsExtensionsPairs.push_back(std::make_pair(m_includePaths, m_includeExtensions));
	pathsExtensionsPairs.push_back(std::make_pair(m_sourcePaths, m_sourceExtensions));

	for (size_t i = 0; i < pathsExtensionsPairs.size(); i++)
	{
		utility::append(
			fileInfos,
			FileSystem::getFileInfosFromPaths(pathsExtensionsPairs[i].first, pathsExtensionsPairs[i].second)
		);
	}

	return fileInfos;
}
