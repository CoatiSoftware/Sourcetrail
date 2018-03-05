#include "utility/commandline/commands/CommandlineCommandAcceptEULA.h"

#include "utility/commandline/CommandlineHelper.h"
#include "utility/commandline/CommandLineParser.h"

namespace commandline
{

CommandlineCommandAcceptEULA::CommandlineCommandAcceptEULA(CommandLineParser* parser)
	: CommandlineCommand("accept-eula", "Accept the Sourcetrail End User License Agreement.", parser)
{
}

CommandlineCommandAcceptEULA::~CommandlineCommandAcceptEULA()
{
}

void CommandlineCommandAcceptEULA::setup()
{
}

void CommandlineCommandAcceptEULA::preparse()
{
	m_parser->acceptEULA();
}

CommandlineCommand::ReturnStatus CommandlineCommandAcceptEULA::parse(std::vector<std::string>& args)
{
	return ReturnStatus::CMD_QUIT;
}

}
