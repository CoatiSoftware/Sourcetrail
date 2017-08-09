#ifndef COMMANDLINE_COMMAND_INDEX_H
#define COMMANDLINE_COMMAND_INDEX_H

#include <memory>
#include "boost/program_options.hpp"

#include "utility/commandline/commands/CommandlineCommand.h"

//#include "utility/commandline/commands/CommandlineSourceGroupOptionsC.h"
//#include "utility/commandline/commands/CommandlineSourceGroupOptionsCxx.h"
//#include "utility/commandline/commands/CommandlineSourceGroupOptionsJava.h"
namespace po = boost::program_options;

namespace commandline {

enum class ReturnStatus;

class CommandIndex : public Command {
public:
	CommandIndex(CommandLineParser* parser);

	virtual void setup();
	virtual ReturnStatus parse(std::vector<std::string>& args);
};

} // namespace cmd

#endif // !COMMANDLINE_COMMAND_H
