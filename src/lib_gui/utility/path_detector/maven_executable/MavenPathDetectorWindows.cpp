#include "MavenPathDetectorWindows.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorWindows::MavenPathDetectorWindows(): PathDetector("Maven for Windows") {}

std::vector<FilePath> MavenPathDetectorWindows::doGetPaths() const
{
	std::vector<FilePath> paths;

	const utility::ProcessOutput out = utility::executeProcess(
		L"cmd", std::vector<std::wstring> {L"/c where mvn.cmd && exit"});

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
