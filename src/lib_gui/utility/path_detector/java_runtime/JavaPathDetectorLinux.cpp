#include "utility/path_detector/java_runtime/JavaPathDetectorLinux.h"

#include "utility/utilityApp.h"
#include "utility/utilityString.h"

#ifdef __x86_64__
	const std::string arch = "amd64";
#else
	const std::string arch = "i386";
#endif

const std::string jvmLibPathRelativeToJavaExecutable = "/../lib/" + arch + "/server/libjvm.so";

JavaPathDetectorLinux::JavaPathDetectorLinux(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Linux", javaVersion)
{
}

JavaPathDetectorLinux::~JavaPathDetectorLinux()
{
}

FilePath JavaPathDetectorLinux::getJavaInPath() const
{
	std::string command = "which java";
	std::string output = utility::executeProcess(command.c_str());

	if (output.size())
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
	FilePath javaPath( utility::executeProcess(command.c_str()));
	if ( !javaPath.empty() )
	{
		return javaPath;
	}
	return FilePath();
}

FilePath JavaPathDetectorLinux::getFilePathRelativeToJavaExecutable(FilePath& javaExecutablePath) const
{
	FilePath p(javaExecutablePath.parentDirectory().str() + jvmLibPathRelativeToJavaExecutable);
	if ( p.exists() )
	{
		return p.canonical();
	}
	else
	{
		return FilePath();
	}
}

FilePath JavaPathDetectorLinux::getJavaInJavaHome() const
{
	std::string command = "";

	char* p = getenv("JAVA_HOME");
	if ( p == nullptr )
	{
		return FilePath();
	}

	FilePath javaPath(std::string(p) + "/bin/java");
	if ( !javaPath.empty() && javaPath.exists() )
	{
		return javaPath;
	}
	return FilePath();
}

bool JavaPathDetectorLinux::checkVersion(const FilePath& path) const
{
	std::string command = path.str() + " -version";
	std::string output = utility::executeProcess(command.c_str());

	return output.find(m_javaVersion) != std::string::npos;
}

std::vector<FilePath> JavaPathDetectorLinux::getPaths() const
{
	std::vector<FilePath> paths;
	FilePath p = getJavaInPath();
	if( !p.empty() )
	{
		paths.push_back(p);
	}
	p = getJavaInJavaHome();
	if( !p.empty() )
	{
		paths.push_back(p);
	}

	// some default paths for java
	paths.push_back(FilePath("/etc/alternatives/java"));
	paths.push_back(FilePath("/usr/lib/jvm/default/bin/java"));
	paths.push_back(FilePath("/usr/lib/jvm/java-openjdk/bin/java"));

	for ( FilePath path : paths )
	{
		if (checkVersion(path))
		{
			FilePath absoluteJavaPath = readLink(path);
			FilePath jvmLibrary = getFilePathRelativeToJavaExecutable(absoluteJavaPath);
			if (jvmLibrary.exists())
			{
				std::vector<FilePath> foundPath = { jvmLibrary };
				return foundPath;
			}
		}
	}

	return std::vector<FilePath>();
}


