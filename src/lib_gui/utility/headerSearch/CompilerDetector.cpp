#include "utility/headerSearch/CompilerDetector.h"

#include "utility/utilityApp.h"
#include "utility/utilityString.h"

CompilerDetector::CompilerDetector(const std::string& name)
	: DetectorBase(name)
{
}

CompilerDetector::~CompilerDetector()
{
}

std::vector<std::string> CompilerDetector::getHeaderPaths()
{
	std::string command = m_name + " -x c++ -v -E /dev/null";
	std::string clangOutput = utility::executeProcess(command.c_str());
	std::string standardHeaders =
		utility::substrBetween(clangOutput, "#include <...> search starts here:\n","\nEnd of search list");
	std::vector<std::string> paths;

	if (!standardHeaders.empty())
	{
		for (std::string s : utility::splitToVector(standardHeaders, '\n'))
		{
			paths.push_back(utility::trim(s));
		}
	}

	return paths;
}

std::vector<FilePath> CompilerDetector::getStandardHeaderPaths()
{
	std::vector<std::string> paths = getHeaderPaths();
	std::vector<FilePath> headerPaths;
	for (const std::string& path : paths)
	{
		if (!utility::isPostfix(" (framework directory)", path))
		{
			headerPaths.push_back(FilePath(path).canonical());
		}
	}

	return headerPaths;
}

std::vector<FilePath> CompilerDetector::getStandardFrameworkPaths()
{
	std::vector<std::string> paths = getHeaderPaths();
	std::vector<FilePath> frameworkPaths;
	for (const std::string& path : paths)
	{
		if (utility::isPostfix(" (framework directory)", path))
		{
			frameworkPaths.push_back(FilePath(utility::replace(path, " (framework directory)", "")).canonical());
		}
	}

	return frameworkPaths;
}
