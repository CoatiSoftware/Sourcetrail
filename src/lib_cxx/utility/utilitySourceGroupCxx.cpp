#include "utilitySourceGroupCxx.h"

std::set<FilePath> utility::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths,
	const std::set<FilePath>& indexedFilePaths,
	const std::set<FilePath>& indexedFileOrDirectoryPaths,
	const std::vector<FilePathFilter>& excludeFilters)
{
	std::set<FilePath> containedFilePaths;

	for (const FilePath& filePath : filePaths)
	{
		bool isInIndexedPaths = false;

		for (const FilePath& indexedFileOrDirectoryPath : indexedFileOrDirectoryPaths)
		{
			if (indexedFileOrDirectoryPath == filePath || indexedFileOrDirectoryPath.contains(filePath))
			{
				isInIndexedPaths = true;
				break;
			}
		}

		if (!isInIndexedPaths && indexedFilePaths.find(filePath) != indexedFilePaths.end())
		{
			isInIndexedPaths = true;
		}

		if (isInIndexedPaths)
		{
			for (const FilePathFilter& excludeFilter : excludeFilters)
			{
				if (excludeFilter.isMatching(filePath))
				{
					isInIndexedPaths = false;
					break;
				}
			}
		}

		if (isInIndexedPaths)
		{
			containedFilePaths.insert(filePath);
		}
	}

	return containedFilePaths;
}
