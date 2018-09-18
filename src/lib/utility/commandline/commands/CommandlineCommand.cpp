#include "CommandlineCommand.h"

#include <iostream>

namespace commandline
{

CommandlineCommand::CommandlineCommand(const std::string& name, const std::string& description, CommandLineParser* parser)
	: m_name(name)
	, m_description(description)
	, m_parser(parser)
{
}

CommandlineCommand::~CommandlineCommand()
{
}

const std::string& CommandlineCommand::name()
{
	return m_name;
}

const std::string& CommandlineCommand::description()
{
	return m_description;
}

void CommandlineCommand::printHelp()
{
	std::cout << "Usage:\n\n  Sourcetrail " << m_name << " [option...]\n\n";
	std::cout << m_description << "\n\n";
	std::cout << m_options << std::endl;

	if (m_positional.max_total_count() > 0)
	{
		std::cout << "Positional Arguments: ";
		for (unsigned int i = 0; i < m_positional.max_total_count(); i++)
		{
			std::cout << "\n  " << i+1 << ": " << m_positional.name_for_position(i);
		}
		std::cout << std::endl;
	}
}

} // namespace commandline
