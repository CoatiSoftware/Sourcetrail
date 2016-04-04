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

std::vector<FilePath> CompilerDetector::getStandardHeaderPaths()
{
    std::string command = m_name + " -x c++ -v -E /dev/null";
    std::string clangOutput = utility::executeProcess(command.c_str());
    std::string standardHeaders = utility::substrBetween(clangOutput, "#include <...> search starts here:\n","\nEnd of search list");
	std::vector<FilePath> paths;

    if (!standardHeaders.empty())
    {
		for (std::string s : utility::splitToVector(standardHeaders, '\n'))
		{
			paths.push_back(s);
		}
    }

	return paths;
}

// Add Gcc to Available Detectors
static StandardHeaderDetection::Add<CompilerDetector> gcc("gcc");
// Add Clang to Available Detectors
static StandardHeaderDetection::Add<CompilerDetector> clang("clang");

