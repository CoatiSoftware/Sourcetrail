#ifndef COMMANDLINE_COMMAND_H
#define COMMANDLINE_COMMAND_H

#include <memory>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace commandline {

class CommandLineParser;

class CommandlineCommand
{
public:
	enum class ReturnStatus {
		CMD_OK,
		CMD_QUIT,
		CMD_FAILURE
	};

	CommandlineCommand(const std::string& name, const std::string& description, CommandLineParser* parser);
	virtual ~CommandlineCommand();

	const std::string& name();
	const std::string& description();

	virtual void setup() = 0;
	virtual ReturnStatus parse(std::vector<std::string>& args) = 0;

	virtual bool hasHelp() const = 0;
	virtual void printHelp();

protected:
	const std::string m_name;
	const std::string m_description;
	CommandLineParser* m_parser;

	po::options_description m_options;
	po::positional_options_description m_positional;
};

}

#endif // COMMANDLINE_COMMAND_H
