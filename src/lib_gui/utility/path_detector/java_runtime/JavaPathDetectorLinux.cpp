#include "JavaPathDetectorLinux.h"

#include "FilePath.h"
#include "utilityApp.h"
#include "utilityString.h"

#ifdef __x86_64__
const wchar_t jvmLibPathRelativeToJavaExecutable[] = L"/../lib/amd64/server/libjvm.so";
#else
const wchar_t jvmLibPathRelativeToJavaExecutable[] = L"/../lib/i386/server/libjvm.so";
#endif


JavaPathDetectorLinux::JavaPathDetectorLinux(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Linux", javaVersion)
{
}

std::vector<FilePath> JavaPathDetectorLinux::doGetPaths() const
{
	std::vector<FilePath> paths;
	FilePath p = getJavaInPath();
	if (!p.empty())
	{
		paths.push_back(p);
	}
	p = getJavaInJavaHome();
	if (!p.empty())
	{
		paths.push_back(p);
	}

	// some default paths for java
	paths.push_back(FilePath(L"/etc/alternatives/java"));
	paths.push_back(FilePath(L"/usr/lib/jvm/default/bin/java"));
	paths.push_back(FilePath(L"/usr/lib/jvm/java-openjdk/bin/java"));

	for (const FilePath& path: paths)
	{
		if (checkVersion(path))
		{
			FilePath absoluteJavaPath = readLink(path);
			FilePath jvmLibrary = getFilePathRelativeToJavaExecutable(absoluteJavaPath);
			if (jvmLibrary.exists())
			{
				std::vector<FilePath> foundPath = {jvmLibrary};
				return foundPath;
			}
		}
	}

	return std::vector<FilePath>();
}

FilePath JavaPathDetectorLinux::getJavaInPath() const
{
	std::string command = "which java";
	std::string output = utility::executeProcess(command.c_str()).second;

	if (!output.empty())
	{
		output = utility::trim(output);

		FilePath javaPath(output);
		if (!javaPath.empty() && javaPath.exists())
		{
			return javaPath;
		}
	}

	return FilePath();
}

FilePath JavaPathDetectorLinux::readLink(const FilePath& path) const
{
	std::string command = "readlink -f " + path.str();
	FilePath javaPath(utility::executeProcess(command.c_str()).second);
	if (!javaPath.empty())
	{
		return javaPath;
	}
	return FilePath();
}

FilePath JavaPathDetectorLinux::getFilePathRelativeToJavaExecutable(FilePath& javaExecutablePath) const
{
	FilePath p = javaExecutablePath.getParentDirectory().concatenate(
		jvmLibPathRelativeToJavaExecutable);
	if (p.exists())
	{
		return p.makeCanonical();
	}
	return FilePath();
}

FilePath JavaPathDetectorLinux::getJavaInJavaHome() const
{
	std::string command = "";
#pragma warning(push)
#pragma warning(disable : 4996)
	char* p = getenv("JAVA_HOME");
#pragma warning(pop)
	if (p == nullptr)
	{
		return FilePath();
	}

	FilePath javaPath(std::string(p) + "/bin/java");
	if (!javaPath.empty() && javaPath.exists())
	{
		return javaPath;
	}
	return FilePath();
}

bool JavaPathDetectorLinux::checkVersion(const FilePath& path) const
{
	std::string command = path.str() + " -version";
	std::string output = utility::executeProcess(command.c_str()).second;

	return output.find(m_javaVersion) != std::string::npos;
}
