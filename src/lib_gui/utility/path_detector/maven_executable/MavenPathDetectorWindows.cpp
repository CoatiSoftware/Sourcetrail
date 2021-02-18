#include "MavenPathDetectorWindows.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorWindows::MavenPathDetectorWindows(): PathDetector("Maven for Windows") {}

std::vector<FilePath> MavenPathDetectorWindows::doGetPaths() const
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
