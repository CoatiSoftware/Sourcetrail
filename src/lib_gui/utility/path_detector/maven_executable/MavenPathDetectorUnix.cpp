#include "MavenPathDetectorUnix.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorUnix::MavenPathDetectorUnix(): PathDetector("Maven for Unix") {}

std::vector<FilePath> MavenPathDetectorUnix::doGetPaths() const
{
	FilePath mavenPath(utility::executeProcess(L"which", std::vector<std::wstring>{L"mvn"}).second);

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
