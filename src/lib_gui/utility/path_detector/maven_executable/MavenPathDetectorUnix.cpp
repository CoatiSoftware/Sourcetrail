#include "MavenPathDetectorUnix.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorUnix::MavenPathDetectorUnix(): PathDetector("Maven for Unix") {}

std::vector<FilePath> MavenPathDetectorUnix::doGetPaths() const
{
	std::vector<FilePath> paths;

	bool ok;
	FilePath mavenPath(utility::searchPath(L"mvn.cmd", ok));
	if (ok && !mavenPath.empty() && mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
