#include "utilityFile.h"

#include "FilePath.h"

#include "utility.h"

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

FilePath utility::getAsRelativeIfShorter(const FilePath& absolutePath, const FilePath& baseDirectory)
{
	if (!baseDirectory.empty())
	{
		const FilePath relativePath = absolutePath.getRelativeTo(baseDirectory);
		if (relativePath.wstr().size() < absolutePath.wstr().size())
		{
			return relativePath;
		}
	}
	return absolutePath;
}

std::vector<FilePath> utility::getAsRelativeIfShorter(const std::vector<FilePath>& absolutePaths, const FilePath& baseDirectory)
{
	return utility::convert<FilePath, FilePath>(absolutePaths, [&](const FilePath& path) { return getAsRelativeIfShorter(path, baseDirectory); });
}

