#include "MavenPathDetectorWindows.h"

#include "FilePath.h"
#include "utilityApp.h"

MavenPathDetectorWindows::MavenPathDetectorWindows(): PathDetector("Maven for Windows") {}

std::vector<FilePath> MavenPathDetectorWindows::doGetPaths() const
{
	FilePath mavenPath(utility::executeProcess(L"cmd", std::vector<std::wstring>{L"/c where mvn.cmd && exit"}).second);

	std::vector<FilePath> paths;
	if (mavenPath.exists())
	{
		paths.push_back(mavenPath);
	}
	return paths;
}
