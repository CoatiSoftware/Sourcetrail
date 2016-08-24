#include "utility/path_detector/cxx_header/utilityCxxHeaderDetection.h"

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
			for (std::string s : utility::splitToVector(standardHeaders, '\n'))
			{
				paths.push_back(utility::trim(s));
			}
		}

		return paths;
	}
}
