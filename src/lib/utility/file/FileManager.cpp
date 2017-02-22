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

void FileManager::setPaths(
	std::vector<FilePath> sourcePaths,
	std::vector<FilePath> headerPaths,
	std::vector<FilePath> excludePaths,
	std::vector<std::string> sourceExtensions
){
	m_sourcePaths = sourcePaths;
	m_headerPaths = makeCanonical(headerPaths);
	m_excludePaths = makeCanonical(excludePaths);
	m_sourceExtensions = sourceExtensions;
}

FileManager::FileSets FileManager::fetchFilePaths(const std::vector<FileInfo>& oldFileInfos)
{
	m_filesInfos.clear();
	for (FileInfo oldFileInfo: oldFileInfos)
	{
		m_filesInfos.emplace(oldFileInfo.path, oldFileInfo);
	}

	FileSets fileSets;

	// update old files that have been modified
	// remove old files that don't exist anymore
	for (std::map<FilePath, FileInfo>::iterator it = m_filesInfos.begin(); it != m_filesInfos.end(); it++)
	{
		const FilePath& filePath = it->first;
		if (filePath.exists() && !hasSourceFilePath(filePath))
		{
			FileInfo newFileInfo = FileSystem::getFileInfoForPath(filePath);

			if (newFileInfo.lastWriteTime > it->second.lastWriteTime)
			{
				it->second.lastWriteTime = newFileInfo.lastWriteTime;
				fileSets.updatedFiles.insert(filePath);
			}
		}
		else
		{
			fileSets.removedFiles.insert(filePath);
		}
	}

	std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromPaths(m_sourcePaths, m_sourceExtensions);
	for (FileInfo fileInfo: fileInfos)
	{
		const FilePath& filePath = fileInfo.path;
		if (isExcluded(filePath))
		{
			continue;
		}

		fileSets.allSourceFilePaths.insert(filePath);

		std::map<FilePath, FileInfo>::iterator it = m_filesInfos.find(filePath);
		if (it != m_filesInfos.end())
		{
			fileSets.removedFiles.erase(filePath);
			if (fileInfo.lastWriteTime > it->second.lastWriteTime)
			{
				it->second.lastWriteTime = fileInfo.lastWriteTime;
				fileSets.updatedFiles.insert(filePath);
			}
		}
		else
		{
			m_filesInfos.insert(std::pair<FilePath, FileInfo>(filePath, fileInfo));
			fileSets.addedFiles.insert(filePath);
		}
	}

	for (const FilePath& filePath : fileSets.removedFiles)
	{
		m_filesInfos.erase(filePath);
	}

	m_sourceFilePaths = fileSets.allSourceFilePaths;

	return fileSets;
}

std::vector<FilePath> FileManager::getSourcePaths() const
{
	return m_sourcePaths;
}

std::set<FilePath> FileManager::getSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;
	for (const FileInfo& fileInfo: FileSystem::getFileInfosFromPaths(m_sourcePaths, m_sourceExtensions))
	{
		sourceFilePaths.emplace(fileInfo.path);
	}
	return sourceFilePaths;
}

bool FileManager::hasSourceFilePath(const FilePath& filePath) const
{
	if (m_sourceFilePaths.find(filePath) != m_sourceFilePaths.end())
	{
		return true;
	}

	return false;
}

std::vector<FilePath> FileManager::makeCanonical(const std::vector<FilePath>& filePaths)
{
	std::vector<FilePath> ret;
	for (const FilePath filePath: filePaths)
	{
		ret.push_back(filePath.canonical());
	}
	return ret;
}

bool FileManager::isExcluded(const FilePath& filePath) const
{
	for (FilePath path : m_excludePaths)
	{
		if (path == filePath || path.contains(filePath))
		{
			return true;
		}
	}

	return false;
}
