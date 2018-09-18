#ifndef COMMANDLINE_COMMAND_ACCEPT_EULA_H
#define COMMANDLINE_COMMAND_ACCEPT_EULA_H

#include "CommandlineCommand.h"

class CommandLineParser;

namespace commandline
{

class CommandlineCommandAcceptEULA
	: public CommandlineCommand
{
public:
	CommandlineCommandAcceptEULA(CommandLineParser* parser);
	virtual ~CommandlineCommandAcceptEULA();

	virtual void setup();
	virtual void preparse();
	virtual ReturnStatus parse(std::vector<std::string>& args);

	virtual bool hasHelp() const { return false; }
};

} // namespace commandline

#endif // COMMANDLINE_COMMAND_ACCEPT_EULA_H
