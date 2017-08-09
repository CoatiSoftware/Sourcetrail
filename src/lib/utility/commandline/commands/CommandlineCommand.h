#ifndef COMMANDLINE_Command_H
#define COMMANDLINE_Command_H

#include <memory>
#include <iostream>
#include "boost/program_options.hpp"

//#include "utility/commandline/commands/CommandlineSourceGroupOptionsC.h"
//#include "utility/commandline/commands/CommandlineSourceGroupOptionsCxx.h"
//#include "utility/commandline/commands/CommandlineSourceGroupOptionsJava.h"

namespace po = boost::program_options;

namespace commandline {
enum class ReturnStatus;
class CommandLineParser;


class Command {
public:
	Command(const std::string name, CommandLineParser* parser = nullptr);

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
//class CommandlineCommandAdd : public CommandlineCommand
//{
//	std::vector<std::shared_ptr<CommandlineSourceGroupOptions>> m_sourceGroupTypes;

//	CommandlineSourceGroupOptionsJava javaSourceGroup;

//	virtual void setup()
//	{
//		// ADD different sourcegroups here
//		m_sourceGroupTypes.push_back(std::make_shared<CommandlineSourceGroupOptionsJava>());


//		for (std::shared_ptr<CommandlineSourceGroupOptions> group : m_sourceGroupTypes)
//		{
//			group->setup();
//		}
//	}

//	virtual void parse(int argc, char** argv)
//	{
//		for (std::shared_ptr<CommandlineSourceGroupOptions> group : m_sourceGroupTypes)
//		{
//			// 0 .. sourcetrail
//			// 1 .. add
//			// 2 .. sourcegrouptype
//			if (argv[2] == group->name())
//			{
//				group->parse(argc, argv);
//			}
//		}
//	}
//};



#endif // !COMMANDLINE_COMMAND_H
