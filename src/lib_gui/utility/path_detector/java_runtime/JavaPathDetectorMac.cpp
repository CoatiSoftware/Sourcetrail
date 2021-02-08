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

	const utility::ProcessOutput out = utility::executeProcess(
		L"/usr/libexec/java_home", std::vector<std::wstring> {});

	const std::wstring output = out.exitCode == 0 ? utility::trim(out.output) : L"";
	if (!output.empty())
	{
		javaPath = FilePath(output + L"/../MacOS/libjli.dylib").makeCanonical();
	}

	if (!javaPath.exists() && !output.empty())
	{
		javaPath = FilePath(output + L"/lib/libjli.dylib");
	}

	if (!javaPath.exists() && !output.empty())
	{
		javaPath = FilePath(output + L"/jre/lib/jli/libjli.dylib");
	}

	if (!javaPath.exists())
	{
		javaPath = FilePath(L"/usr/lib/libjli.dylib");
	}

	if (!javaPath.exists() && !output.empty())
	{
		javaPath = FilePath(output + L"/jre/lib/server/libjvm.dylib");
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
