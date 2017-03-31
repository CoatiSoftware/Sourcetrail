#include "utility/file/FileManager.h"

#include <set>

#include "utility/file/FileSystem.h"
#include "utility/utility.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

void FileManager::update(
	const std::vector<FilePath>& sourcePaths,
	const std::vector<FilePath>& excludePaths,
	const std::vector<std::string>& sourceExtensions
){
	m_sourcePaths = sourcePaths;
	m_excludePaths = makeCanonical(excludePaths);
	m_sourceExtensions = sourceExtensions;

	m_allSourceFilePaths.clear();

	for (FileInfo fileInfo: FileSystem::getFileInfosFromPaths(m_sourcePaths, m_sourceExtensions))
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
