#include "MavenPathDetectorUnix.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorUnix::MavenPathDetectorUnix(): PathDetector("Maven for Unix") {}

std::vector<FilePath> MavenPathDetectorUnix::doGetPaths() const
{
	std::vector<FilePath> paths;

	const utility::ProcessOutput out = utility::executeProcess(L"which", {L"mvn"});

	if (out.exitCode == 0)
	{
		FilePath mavenPath(out.output);
		if (mavenPath.exists())
		{
			paths.push_back(mavenPath);
		}
	}
	return paths;
}
