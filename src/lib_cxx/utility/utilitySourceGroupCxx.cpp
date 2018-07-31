#include "utility/utilitySourceGroupCxx.h"

std::set<FilePath> utility::filterToContainedFilePaths(
	const std::set<FilePath> filePaths,
	const std::set<FilePath>& indexedSourcePaths,
	const std::set<FilePath>& indexedHeaderPaths,
	const std::vector<FilePathFilter> excludeFilters)
{
	std::set<FilePath> containedFilePaths;

	for (const FilePath& filePath : filePaths)
	{
		bool isInIndexedPaths = false;
		{
			for (const FilePath& indexedHeaderPath : indexedHeaderPaths)
			{
				if (indexedHeaderPath == filePath || indexedHeaderPath.contains(filePath))
				{
					isInIndexedPaths = true;
					break;
				}
			}
		}
		if (!isInIndexedPaths)
		{
			for (const FilePath& indexedSourcePath : indexedSourcePaths)
			{
				if (indexedSourcePath == filePath)
				{
					isInIndexedPaths = true;
					break;
				}
			}
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
