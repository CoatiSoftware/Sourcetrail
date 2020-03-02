#include "MavenPathDetectorUnix.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorUnix::MavenPathDetectorUnix(): PathDetector("Maven for Unix") {}

std::vector<FilePath> MavenPathDetectorUnix::doGetPaths() const
{
	std::string command = "which mvn";
	FilePath mavenPath(utility::executeProcess(command.c_str()).second);

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
