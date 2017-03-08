#include "utility/path_detector/maven_executable/MavenPathDetectorUnix.h"

#include "utility/utilityApp.h"

MavenPathDetectorUnix::MavenPathDetectorUnix()
	: PathDetector("Maven for Unix")
{
}

MavenPathDetectorUnix::~MavenPathDetectorUnix()
{
}

std::vector<FilePath> MavenPathDetectorUnix::getPaths() const
{
	std::string command = "which mvn";
	FilePath mavenPath(utility::executeProcess(command.c_str()));

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
