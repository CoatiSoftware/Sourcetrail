#ifndef COMMANDLINE_SOURCEGROUP_OPTIONS_H
#define COMMANDLINE_SOURCEGROUP_OPTIONS_H

#include <string>

#include "boost/program_options.hpp"

class CommandlineSourceGroupOptions
{
public:
	virtual const std::string name() const = 0;
	virtual void setup() = 0;
	virtual void parse(int argc, char** argv) = 0;

protected:
	boost::program_options::options_description m_options;
};

#endif // COMMANDLINE_SOURCEGROUP_OPTIONS_H
