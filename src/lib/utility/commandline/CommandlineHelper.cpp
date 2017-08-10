#include "utility/commandline/CommandlineHelper.h"

#include <fstream>
#include <iostream>
#include <string>

#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace commandline {

void parseConfigFile(po::variables_map& vm,
					 po::options_description& options)
{
	if (vm.count("config-file"))
	{
		const std::string configFile = vm["config-file"].as<std::string>();
		std::ifstream ifs(configFile.c_str());

		if (!ifs)
		{
			std::cout << "Could not open config file( "
					<< configFile << ")"
					<< std::endl;
		}
		else
		{
			po::store(po::parse_config_file(ifs, options), vm);
		}
	}
}

void conflicting_options(const boost::program_options::variables_map& vm,
						 const char* opt1, const char* opt2)
{
	if (vm.count(opt1) && !vm[opt1].defaulted()
		&& vm.count(opt2) && !vm[opt2].defaulted())
	{
		std::cout << "Options " << opt1
				  << " and " << opt2
				  << " are conflicted." << std::endl;
	}
}

std::vector<FilePath> extractPaths(const std::vector<std::string>& vector)
{
	std::vector<FilePath> v;
	for (const std::string& s : vector)
	{
		std::vector<std::string> temp= utility::splitToVector(s, ',');
		for (const std::string& path : temp)
		{
			v.push_back(FilePath(path));
		}
	}
	return v;
}

} // namespace cmd
