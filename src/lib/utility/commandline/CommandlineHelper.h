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

namespace po = boost::program_options;

namespace commandline
{
	enum class ReturnStatus {
		CMD_OK,
		CMD_QUIT,
		CMD_FAILURE
	};

	void parseConfigFile(po::variables_map& vm, po::options_description& options);

	std::vector<FilePath> extractPaths(const std::vector<std::string>& vector);
}

#endif // COMMANDLINE_HELPER_H
