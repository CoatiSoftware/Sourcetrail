#include "utility/file/FileManager.h"

#include <set>

#include "utility/file/FileSystem.h"
#include "utility/file/FilePath.h"

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

std::set<FilePath> FileManager::getAllSourceFilePathsRelative(const FilePath& baseDirectory) const
{
	std::set<FilePath> absolutePaths;
	for (const FilePath& path: getAllSourceFilePaths())
	{
		if (baseDirectory.exists())
		{
			absolutePaths.insert(path.getRelativeTo(baseDirectory));
		}
		else
		{
			absolutePaths.insert(path);
		}
	}
	return absolutePaths;
}

std::vector<FilePath> FileManager::makeCanonical(const std::vector<FilePath>& filePaths)
{
	std::vector<FilePath> ret;
	for (const FilePath& filePath: filePaths)
	{
		ret.push_back(filePath.getCanonical());
	}
	return ret;
}

bool FileManager::isExcluded(const FilePath& filePath) const
{
	for (const FilePath& path : m_excludePaths)
	{
		if (path == filePath || path.contains(filePath))
		{
			return true;
		}
	}

	return false;
}
