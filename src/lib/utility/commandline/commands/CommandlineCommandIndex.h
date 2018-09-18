#ifndef COMMANDLINE_COMMAND_INDEX_H
#define COMMANDLINE_COMMAND_INDEX_H

#include "CommandlineCommand.h"

namespace commandline {

class CommandlineCommandIndex
	: public CommandlineCommand
{
public:
	CommandlineCommandIndex(CommandLineParser* parser);
	virtual ~CommandlineCommandIndex();

	virtual void setup();
	virtual void preparse();
	virtual ReturnStatus parse(std::vector<std::string>& args);

	virtual bool hasHelp() const { return true; }
};

} // namespace cmd

#endif // COMMANDLINE_COMMAND_INDEX_H
