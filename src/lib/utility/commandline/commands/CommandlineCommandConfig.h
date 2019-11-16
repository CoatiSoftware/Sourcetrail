#ifndef COMMANDLINE_COMMAND_CONFIG_H
#define COMMANDLINE_COMMAND_CONFIG_H

#include "CommandlineCommand.h"

namespace commandline
{

class CommandlineCommandConfig
	: public CommandlineCommand
{
public:
	CommandlineCommandConfig(CommandLineParser* parser);
	virtual ~CommandlineCommandConfig();

	virtual void setup();
	virtual ReturnStatus parse(std::vector<std::string>& args);

	virtual bool hasHelp() const { return true; }
};

} // namespace commandline

#endif // COMMANDLINE_COMMAND_CONFIG_H
