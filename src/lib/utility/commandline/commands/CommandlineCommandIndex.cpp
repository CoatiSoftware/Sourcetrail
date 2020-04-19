#include "CommandlineCommandIndex.h"

#include <iostream>

#include "CommandLineParser.h"
#include "CommandlineHelper.h"

namespace po = boost::program_options;

namespace commandline
{
CommandlineCommandIndex::CommandlineCommandIndex(CommandLineParser* parser)
	: CommandlineCommand("index", "Index a certain project.", parser)
{
}

CommandlineCommandIndex::~CommandlineCommandIndex() {}

void CommandlineCommandIndex::setup()
{
	po::options_description options("Config Options");
	options.add_options()("help,h", "Print this help message")(
		"incomplete,i", "Also reindex incomplete files (files with errors)")(
		"full,f", "Index full project (omit to only index new/changed files)")(
		"shallow,s", "Build a shallow index is supported by the project")(
		"project-file", po::value<std::string>(), "Project file to index (.srctrlprj)");

	m_options.add(options);
	m_positional.add("project-file", 1);
}

CommandlineCommand::ReturnStatus CommandlineCommandIndex::parse(std::vector<std::string>& args)
{
	po::variables_map vm;
	try
	{
		po::store(
			po::command_line_parser(args).options(m_options).positional(m_positional).run(), vm);
		po::notify(vm);

		parseConfigFile(vm, m_options);
	}
	catch (po::error& e)
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
		m_parser->fullRefresh();
	}
	else if (vm.count("incomplete"))
	{
		m_parser->incompleteRefresh();
	}

	if (vm.count("shallow"))
	{
		m_parser->setShallowIndexingRequested();
	}

	if (vm.count("project-file"))
	{
		m_parser->setProjectFile(FilePath(vm["project-file"].as<std::string>()));
	}

	return ReturnStatus::CMD_OK;
}

}	 // namespace commandline
