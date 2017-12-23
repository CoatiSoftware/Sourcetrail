#include "utility/path_detector/cxx_header/utilityCxxHeaderDetection.h"

#include <QSettings>
#include <QSysInfo>

#include "utility/file/FileSystem.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"

namespace utility
{
	std::vector<std::string> getCxxHeaderPaths(const std::string& compilerName)
	{
		std::string command = compilerName + " -x c++ -v -E /dev/null";
		std::string clangOutput = utility::executeProcess(command.c_str());
		std::string standardHeaders =
			utility::substrBetween(clangOutput, "#include <...> search starts here:\n","\nEnd of search list");
		std::vector<std::string> paths;

		if (!standardHeaders.empty())
		{
			for (const std::string& s : utility::splitToVector(standardHeaders, '\n'))
			{
				paths.push_back(utility::trim(s));
			}
		}

		return paths;
	}

	std::vector<FilePath> getWindowsSdkHeaderSearchPaths(ApplicationArchitectureType architectureType)
	{
		std::vector<FilePath> headerSearchPaths;

		std::vector<std::string> windowsSdkVersions;
		windowsSdkVersions.push_back("v8.1A");
		windowsSdkVersions.push_back("v8.1");
		windowsSdkVersions.push_back("v8.0A");
		windowsSdkVersions.push_back("v7.1A");
		windowsSdkVersions.push_back("v7.0A");

		for (size_t i = 0; i < windowsSdkVersions.size(); i++)
		{
			const FilePath sdkPath = getWindowsSdkRootPathUsingRegistry(architectureType, windowsSdkVersions[i]);
			if (sdkPath.exists())
			{
				const FilePath sdkIncludePath = sdkPath.getConcatenated(FilePath("include/"));
				if (sdkIncludePath.exists())
				{
					std::vector<std::string> subdirectories;
					subdirectories.push_back("shared");
					subdirectories.push_back("um");
					subdirectories.push_back("winrt");

					bool usingSubdirectories = false;
					for (size_t j = 0; j < subdirectories.size(); j++)
					{
						const FilePath sdkSubdirectory = sdkIncludePath.getConcatenated(FilePath(subdirectories[j]));
						if (sdkSubdirectory.exists())
						{
							headerSearchPaths.push_back(sdkSubdirectory);
							usingSubdirectories = true;
						}
					}

					if (!usingSubdirectories)
					{
						headerSearchPaths.push_back(sdkIncludePath);
					}
					break;
				}
			}
		}
		{
			const FilePath sdkPath = getWindowsSdkRootPathUsingRegistry(architectureType, "v10.0");
			if (sdkPath.exists())
			{
				for (const FilePath& versionPath : FileSystem::getDirectSubDirectories(sdkPath.getConcatenated(FilePath("include/"))))
				{
					const FilePath ucrtPath = versionPath.getConcatenated(FilePath("ucrt"));
					if (ucrtPath.exists())
					{
						headerSearchPaths.push_back(ucrtPath);
						break;
					}
				}
			}
		}

		return headerSearchPaths;
	}

	FilePath getWindowsSdkRootPathUsingRegistry(ApplicationArchitectureType architectureType, const std::string& sdkVersion)
	{
		QString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";
		if (architectureType == APPLICATION_ARCHITECTURE_X86_32)
		{
			key += "Wow6432Node\\";
		}
		key += ("Microsoft\\Microsoft SDKs\\Windows\\" + sdkVersion).c_str();

		QSettings expressKey(key, QSettings::NativeFormat); // NativeFormat means from Registry on Windows.
		QString value = expressKey.value("InstallationFolder").toString();

		FilePath path(value.toStdString());
		if (path.exists())
		{
			return path;
		}

		return FilePath();
	}
}
