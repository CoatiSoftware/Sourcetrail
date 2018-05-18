#include "utility/utilityFile.h"

#include "utility/file/FilePath.h"

#include "utility/utility.h"

std::vector<FilePath> utility::getTopLevelPaths(const std::vector<FilePath>& paths)
{
	return utility::getTopLevelPaths(utility::toSet(paths));
}

std::vector<FilePath> utility::getTopLevelPaths(const std::set<FilePath>& paths)
{
	// this works because the set contains the paths already in alphabetical order
	std::vector<FilePath> topLevelPaths;

	FilePath lastPath;
	for (const FilePath& path : paths)
	{
		if (lastPath.empty() || !lastPath.contains(path)) // don't add subdirectories of already added paths
		{
			lastPath = path;
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}
