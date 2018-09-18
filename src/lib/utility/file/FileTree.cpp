#include "FileTree.h"

#include "FileSystem.h"

FileTree::FileTree(const FilePath& rootPath)
	: m_rootPath(rootPath.getAbsolute().makeCanonical())
{
	if (m_rootPath.exists())
	{
		if (m_rootPath.isDirectory())
		{
			for (const FilePath& filePath : FileSystem::getFilePathsFromDirectory(m_rootPath))
			{
				m_files[filePath.fileName()].insert(filePath);
			}
		}
		else
		{
			m_files[m_rootPath.fileName()].insert(m_rootPath);
		}
	}
}

FilePath FileTree::getAbsoluteRootPathForRelativeFilePath(const FilePath& relativeFilePath)
{
	std::vector<FilePath> rootPaths = doGetAbsoluteRootPathsForRelativeFilePath(relativeFilePath, false);
	if (!rootPaths.empty())
	{
		return rootPaths.front();
	}
	return FilePath();
}

std::vector<FilePath> FileTree::getAbsoluteRootPathsForRelativeFilePath(const FilePath& relativeFilePath)
{
	return doGetAbsoluteRootPathsForRelativeFilePath(relativeFilePath, true);
}

std::vector<FilePath> FileTree::doGetAbsoluteRootPathsForRelativeFilePath(const FilePath& relativeFilePath, bool allowMultipleResults)
{
	std::vector<FilePath> rootPaths;

	std::unordered_map<std::wstring, std::set<FilePath>>::const_iterator it = m_files.find(relativeFilePath.fileName());
	if (it != m_files.end())
	{
		for (FilePath existingFilePath : it->second)
		{
			existingFilePath = existingFilePath.getParentDirectory();
			bool ok = true;
			{
				FilePath temp = relativeFilePath.getParentDirectory();
				while (!temp.empty())
				{
					if (temp.fileName() == L"..")
					{
						std::vector<FilePath> subDirectories = FileSystem::getDirectSubDirectories(existingFilePath);
						if (!subDirectories.empty())
						{
							existingFilePath = subDirectories.front();
						}
						else
						{
							ok = false;
							break;
						}
					}
					else
					{
						existingFilePath = existingFilePath.getParentDirectory();
					}
					temp = temp.getParentDirectory();
				}
			}
			if (ok)
			{
				rootPaths.push_back(existingFilePath);
				if (!allowMultipleResults)
				{
					break;
				}
			}
		}
	}
	return rootPaths;
}

