#include "MavenPathDetectorWindows.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorWindows::MavenPathDetectorWindows(): PathDetector("Maven for Windows") {}

std::vector<FilePath> MavenPathDetectorWindows::doGetPaths() const
{
	std::string command = "cmd /c where mvn.cmd && exit";
	FilePath mavenPath(utility::executeProcess(command.c_str()).second);

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
