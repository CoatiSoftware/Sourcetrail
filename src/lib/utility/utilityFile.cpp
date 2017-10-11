#include "utility/utilityFile.h"

std::vector<FilePath> utility::getTopLevelPaths(const std::vector<FilePath>& paths)
{
	std::vector<FilePath> topLevelPaths;
	for (const FilePath& path : paths)
	{
		bool addPath = true;
		for (size_t i = 0; i < topLevelPaths.size(); i++)
		{
			if (topLevelPaths[i].contains(path))
			{
				addPath = false;
				break;
			}
			else if(path.contains(topLevelPaths[i]))
			{
				topLevelPaths.erase(topLevelPaths.begin() + i);
				break;
			}
		}
		if (addPath)
		{
			topLevelPaths.push_back(path);
		}
	}
	return topLevelPaths;
}
