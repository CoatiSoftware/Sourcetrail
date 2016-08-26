#include "utility/path_detector/java_runtime/JavaPathDetectorMac.h"

#include "utility/utilityApp.h"
#include "utility/utilityString.h"

JavaPathDetectorMac::JavaPathDetectorMac(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Mac", javaVersion)
{
}

JavaPathDetectorMac::~JavaPathDetectorMac()
{
}

std::vector<FilePath> JavaPathDetectorMac::getPaths() const
{
	std::vector<FilePath> paths;

	std::string command = "/usr/libexec/java_home";
	std::string output = utility::executeProcess(command.c_str());

	if (output.size())
	{
		output = utility::trim(output);

		FilePath javaPath(output);
		if (!javaPath.empty() && javaPath.exists())
		{
			paths.push_back(javaPath);
		}
	}

	return paths;
}
