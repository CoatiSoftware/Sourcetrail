#ifndef COMMANDLINE_COMMAND_H
#define COMMANDLINE_COMMAND_H

#include <memory>
#include <iostream>
#include "boost/program_options.hpp"

namespace po = boost::program_options;

namespace commandline {

enum class ReturnStatus;
class CommandLineParser;

class Command
{
public:
	Command(const std::string name, CommandLineParser* parser = nullptr);
	virtual ~Command();

	const std::string name();

	virtual void setup() = 0;
	virtual ReturnStatus parse(std::vector<std::string>& args) = 0;
	virtual void printHelp();

protected:
	const std::string m_name;
	po::options_description m_options;
	po::positional_options_description m_positional;
	CommandLineParser* m_parser;
};

}

#endif // COMMANDLINE_COMMAND_H
