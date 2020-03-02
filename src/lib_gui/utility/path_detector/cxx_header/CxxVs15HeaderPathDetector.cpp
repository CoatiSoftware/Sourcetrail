#include "CxxVs15HeaderPathDetector.h"

#include <string>

#include "FilePath.h"
#include "FileSystem.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityCxxHeaderDetection.h"

CxxVs15HeaderPathDetector::CxxVs15HeaderPathDetector(): PathDetector("Visual Studio 2017") {}

std::vector<FilePath> CxxVs15HeaderPathDetector::doGetPaths() const
{
	std::vector<FilePath> headerSearchPaths;

	{
		const std::vector<FilePath> expandedPaths =
			FilePath(L"%ProgramFiles(x86)%/Microsoft Visual Studio/Installer/vswhere.exe")
				.expandEnvironmentVariables();
		if (!expandedPaths.empty())
		{
			const std::string command = "\"" + expandedPaths[0].str() +
				"\" -latest -property installationPath";
			const std::string command2 =
				"\"C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe\"";
			const std::string output = utility::executeProcess(command, FilePath(), 10000).second;

			const FilePath vsInstallPath(output);
			if (vsInstallPath.exists())
			{
				for (const FilePath& versionPath: FileSystem::getDirectSubDirectories(
						 vsInstallPath.getConcatenated(L"VC/Tools/MSVC")))
				{
					if (versionPath.exists())
					{
						headerSearchPaths.push_back(versionPath.getConcatenated(L"include"));
						headerSearchPaths.push_back(versionPath.getConcatenated(L"atlmfc/include"));
					}
				}
				headerSearchPaths.push_back(
					vsInstallPath.getConcatenated(L"VC/Auxiliary/VS/include"));
				headerSearchPaths.push_back(
					vsInstallPath.getConcatenated(L"VC/Auxiliary/VS/UnitTest/include"));
			}
		}
	}

	if (!headerSearchPaths.empty())
	{
		std::vector<FilePath> windowsSdkHeaderSearchPaths = utility::getWindowsSdkHeaderSearchPaths(
			APPLICATION_ARCHITECTURE_X86_32);
		if (windowsSdkHeaderSearchPaths.empty())
		{
			windowsSdkHeaderSearchPaths = utility::getWindowsSdkHeaderSearchPaths(
				APPLICATION_ARCHITECTURE_X86_64);
		}
		utility::append(headerSearchPaths, windowsSdkHeaderSearchPaths);
	}

	return headerSearchPaths;
}
