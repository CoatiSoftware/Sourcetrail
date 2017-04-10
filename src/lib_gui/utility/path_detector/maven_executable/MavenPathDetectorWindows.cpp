#include "utility/path_detector/maven_executable/MavenPathDetectorWindows.h"

#include "utility/utilityApp.h"

MavenPathDetectorWindows::MavenPathDetectorWindows()
	: PathDetector("Maven for Windows")
{
}

MavenPathDetectorWindows::~MavenPathDetectorWindows()
{
}

std::vector<FilePath> MavenPathDetectorWindows::getPaths() const
{
	std::string command = "cmd /c where mvn.cmd && exit";
	FilePath mavenPath(utility::executeProcess(command.c_str()));

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
