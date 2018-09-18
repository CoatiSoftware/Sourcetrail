#include "FileManager.h"

#include <set>

#include "FileSystem.h"
#include "FilePath.h"
#include "FilePathFilter.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

void FileManager::update(
	const std::vector<FilePath>& sourcePaths,
	const std::vector<FilePathFilter>& excludeFilters,
	const std::vector<std::wstring>& sourceExtensions
){
	m_sourcePaths = sourcePaths;
	m_excludeFilters = excludeFilters;
	m_sourceExtensions = sourceExtensions;

	m_allSourceFilePaths.clear();

	for (const FileInfo& fileInfo : FileSystem::getFileInfosFromPaths(m_sourcePaths, m_sourceExtensions))
	{
		const FilePath& filePath = fileInfo.path;
		if (isExcluded(filePath))
		{
			continue;
		}

		m_allSourceFilePaths.insert(filePath);
	}
}

std::vector<FilePath> FileManager::getSourcePaths() const
{
	return m_sourcePaths;
}

bool FileManager::hasSourceFilePath(const FilePath& filePath) const
{
	if (m_allSourceFilePaths.find(filePath) != m_allSourceFilePaths.end())
	{
		return true;
	}

	return false;
}

std::set<FilePath> FileManager::getAllSourceFilePaths() const
{
	return m_allSourceFilePaths;
}

bool FileManager::isExcluded(const FilePath& filePath) const
{
	for (const FilePathFilter& filter : m_excludeFilters)
	{
		if (filter.isMatching(filePath))
		{
			return true;
		}
	}

	return false;
}
