#ifndef COMMANDLINE_HELPER_H
#define COMMANDLINE_HELPER_H

#include <string>
#include <vector>

#include "../utility.h"
#include "../../../lib_utility/utility/utilityString.h"

namespace boost
{
namespace program_options
{
class variables_map;
class options_description;
}	 // namespace program_options
}	 // namespace boost

namespace po = boost::program_options;

namespace commandline
{
void parseConfigFile(po::variables_map& vm, po::options_description& options);

std::vector<FilePath> extractPaths(const std::vector<std::string>& vector);
}	 // namespace commandline

#endif	  // COMMANDLINE_HELPER_H
