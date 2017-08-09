#include "utility/commandline/commands/CommandlineCommandIndex.h"

#include "utility/commandline/CommandlineHelper.h"
#include "utility/commandline/CommandLineParser.h"

namespace commandline
{

CommandIndex::CommandIndex(CommandLineParser* parser)
	: Command("index", parser)
{
}

void CommandIndex::setup()
{
	po::options_description options("Config Options");
	options.add_options()
		("help,h", "Print this help message")
		("full", "Paths for Sourcefiles")
		("project-file", po::value<std::string>(), "Project file to index")
		;

	m_options.add(options);
	m_positional.add("project-file", 1);
}

ReturnStatus CommandIndex::parse(std::vector<std::string>& args)
{
	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(args).options(m_options).positional(m_positional).run(), vm);
		po::notify(vm);

		parseConfigFile(vm, m_options);
	}
	catch(po::error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << m_options << std::endl;
		return ReturnStatus::CMD_FAILURE;
	}

	if (vm.count("help") || args.size() == 0 || args[0] == "help")
	{
		printHelp();
		return ReturnStatus::CMD_QUIT;
	}

	if (vm.count("full"))
	{
		m_parser->force();
	}

	if (vm.count("project-file"))
	{
		m_parser->setProjectFile(FilePath(vm["project-file"].as<std::string>()));
	}

	return ReturnStatus::CMD_OK;
}

}
