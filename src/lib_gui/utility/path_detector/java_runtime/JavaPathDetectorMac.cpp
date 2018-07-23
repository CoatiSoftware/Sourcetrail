#include "utility/path_detector/java_runtime/JavaPathDetectorMac.h"

#include "utility/file/FilePath.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"

JavaPathDetectorMac::JavaPathDetectorMac(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Mac", javaVersion)
{
}

std::vector<FilePath> JavaPathDetectorMac::getPaths() const
{
	std::vector<FilePath> paths;
	FilePath javaPath;

	std::string command = "/usr/libexec/java_home";
	std::string output = utility::executeProcess(command.c_str());

	if (!output.empty())
	{
		javaPath = FilePath(utility::trim(output) + "/jre/lib/jli/libjli.dylib");
	}

	if (!javaPath.exists())
	{
		javaPath = FilePath(L"/usr/lib/libjli.dylib");
	}

	if (!javaPath.exists() && output.size())
	{
		javaPath = FilePath(utility::trim(output) + "/jre/lib/server/libjvm.dylib");
	}

	if (!javaPath.exists())
	{
		javaPath = FilePath(L"/usr/lib/libjvm.dylib");
	}

	if (javaPath.exists())
	{
		paths.push_back(javaPath);
	}

	return paths;
}
