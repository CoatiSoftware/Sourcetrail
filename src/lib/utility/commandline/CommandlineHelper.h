#ifndef COMMANDLINE_HELPER_H
#define COMMANDLINE_HELPER_H

#include <string>
#include <vector>

#include "utility/utilityString.h"
#include "utility/utility.h"

namespace boost {
namespace program_options {
	class variables_map;
	class options_description;
}
}

namespace commandline
{
	enum class ReturnStatus {
		CMD_OK,
		CMD_QUIT,
		CMD_FAILURE
	};

	void parseConfigFile(boost::program_options::variables_map& vm,
						 boost::program_options::options_description& options);

	void conflicting_options(const boost::program_options::variables_map& vm,
							 const char* opt1, const char* opt2);

	std::vector<FilePath> extractPaths(const std::vector<std::string>& vector);

}

#endif // COMMANDLINE_HELPER_H
