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

void FileManager::setPaths(
	std::vector<FilePath> sourcePaths,
	std::vector<FilePath> headerPaths,
	std::vector<FilePath> excludePaths,
	std::vector<std::string> sourceExtensions
){
	m_sourcePaths = sourcePaths;
	m_headerPaths = headerPaths;
	m_excludePaths = excludePaths;
	m_sourceExtensions = sourceExtensions;
}

void FileManager::fetchFilePaths(const std::vector<FileInfo>& oldFileInfos)
{
	m_files.clear();
	for (FileInfo oldFileInfo: oldFileInfos)
	{
		m_files.emplace(oldFileInfo.path, oldFileInfo);
	}

	m_addedFiles.clear();
	m_updatedFiles.clear();
	m_removedFiles.clear();

	for (std::map<FilePath, FileInfo>::iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		const FilePath& filePath = it->first;
		if (filePath.exists() && !hasSourceExtension(filePath))
		{
			FileInfo fileInfo = FileSystem::getFileInfoForPath(filePath);

			if (fileInfo.lastWriteTime > it->second.lastWriteTime)
			{
				it->second.lastWriteTime = fileInfo.lastWriteTime;
				m_updatedFiles.insert(filePath);
			}
		}
		else
		{
			m_removedFiles.insert(filePath);
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

		std::map<FilePath, FileInfo>::iterator it = m_files.find(filePath);
		if (it != m_files.end())
		{
			m_removedFiles.erase(filePath);
			if (fileInfo.lastWriteTime > it->second.lastWriteTime)
			{
				it->second.lastWriteTime = fileInfo.lastWriteTime;
				m_updatedFiles.insert(filePath);
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
	if (m_files.find(filePath) != m_files.end())
	{
		return true;
	}

	if (isExcluded(filePath))
	{
		return false;
	}

	for (FilePath path : m_headerPaths)
	{
		if (path == filePath || path.contains(filePath))
		{
			return true;
		}
	}

	return false;
}

bool FileManager::hasSourceExtension(const FilePath& filePath) const
{
	return filePath.hasExtension(m_sourceExtensions);
}

const FileInfo FileManager::getFileInfo(const FilePath& filePath) const
{
	std::map<FilePath, FileInfo>::const_iterator it = m_files.find(filePath);

	if (it == m_files.end())
	{
		return FileSystem::getFileInfoForPath(filePath);
	}

	return it->second;
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
