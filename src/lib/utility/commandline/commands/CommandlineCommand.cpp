#include "utility/commandline/commands/CommandlineCommand.h"

#include <iostream>

namespace commandline
{

Command::Command(const std::string name, CommandLineParser* parser)
	: m_name(name)
	, m_parser(parser)
{
}

const std::string Command::name()
{
	return m_name;
}

void Command::printHelp()
{
	std::cout << "Usage:\n  sourcetrail "
			<< m_name << " [option...]\n\n";
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


} // namespace cmd
