#include "PathDetector.h"

#include "FilePath.h"

PathDetector::PathDetector(const std::string& name): m_name(name) {}

std::string PathDetector::getName() const
{
	return m_name;
}

std::vector<FilePath> PathDetector::getPaths() const
{
	std::vector<FilePath> paths;
	for (const FilePath& path: doGetPaths())
	{
		if (path.exists())
		{
			paths.push_back(path);
		}
	}
	return paths;
}

bool PathDetector::isWorking() const
{
	return (!getPaths().empty());
}
