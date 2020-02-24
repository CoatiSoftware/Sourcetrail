#include "JavaPathDetectorMac.h"

#include "FilePath.h"
#include "utilityApp.h"
#include "utilityString.h"

JavaPathDetectorMac::JavaPathDetectorMac(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Mac", javaVersion)
{
}

std::vector<FilePath> JavaPathDetectorMac::doGetPaths() const
{
	std::vector<FilePath> paths;
	FilePath javaPath;

	std::string command = "/usr/libexec/java_home";
	std::string output = utility::executeProcess(command.c_str()).second;

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
