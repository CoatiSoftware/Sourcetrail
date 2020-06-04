#include "CommandLineParser.h"

#include <iostream>
#include <utility>

#include <boost/program_options.hpp>

#include "CommandlineCommandConfig.h"
#include "CommandlineCommandIndex.h"
#include "CommandlineHelper.h"
#include "ConfigManager.h"
#include "TextAccess.h"
#include "utilityString.h"

namespace po = boost::program_options;

namespace commandline
{
CommandLineParser::CommandLineParser(const std::string& version): m_version(version)
{
	po::options_description options("Options");
	options.add_options()("help,h", "Print this help message")(
		"version,v", "Version of Sourcetrail")(
		"project-file", po::value<std::string>(), "Open Sourcetrail with this project (.srctrlprj)");

	m_options.add(options);
	m_positional.add("project-file", 1);

	m_commands.push_back(std::make_unique<commandline::CommandlineCommandConfig>(this));
	m_commands.push_back(std::make_unique<commandline::CommandlineCommandIndex>(this));

	for (auto& command: m_commands)
	{
		command->setup();
	}
}

CommandLineParser::~CommandLineParser() {}

void CommandLineParser::preparse(int argc, char** argv)
{
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++)
	{
		args.push_back(std::string(argv[i]));
	}

	preparse(args);
}

void CommandLineParser::preparse(std::vector<std::string>& args)
{
	if (args.size() < 1)
	{
		return;
	}

	m_args = args;

	try
	{
		for (auto& command: m_commands)
		{
			if (m_args[0] == command->name())
			{
				m_withoutGUI = true;
				return;
			}
		}

		po::variables_map vm;
		po::positional_options_description positional;
		positional.add("project-file", 1);
		po::store(
			po::command_line_parser(m_args)
				.options(m_options)
				.positional(positional)
				.allow_unregistered()
				.run(),
			vm);
		po::notify(vm);

		if (vm.count("version"))
		{
			std::cout << "Sourcetrail Version " << m_version << std::endl;
			m_quit = true;
			return;
		}

		if (vm.count("help"))
		{
			printHelp();
			m_quit = true;
		}

		if (vm.count("project-file"))
		{
			m_projectFile = FilePath(vm["project-file"].as<std::string>());
			processProjectfile();
		}
	}
	catch (boost::program_options::error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << m_options << std::endl;
	}
}

void CommandLineParser::parse()
{
	if (m_args.size() < 1)
	{
		return;
	}

	try
	{
		// parsing for all commands
		for (auto& command: m_commands)
		{
			if (m_args[0] == command->name())
			{
				m_args.erase(m_args.begin());
				CommandlineCommand::ReturnStatus status = command->parse(m_args);

				if (status != CommandlineCommand::ReturnStatus::CMD_OK)
				{
					m_quit = true;
				}
			}
		}
	}
	catch (boost::program_options::error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << m_options << std::endl;
	}
}

void CommandLineParser::setProjectFile(const FilePath& filepath)
{
	m_projectFile = filepath;
	processProjectfile();
}

void CommandLineParser::printHelp() const
{
	std::cout << "Usage:\n  Sourcetrail [command] [option...] [positional arguments]\n\n";

	// Commands
	std::cout << "Commands:\n";
	for (auto& command: m_commands)
	{
		std::cout << "  " << command->name();
		std::cout << std::string(std::max(23 - command->name().size(), size_t(2)), ' ');
		std::cout << command->description() << (command->hasHelp() ? "*" : "") << "\n";
	}
	std::cout << "\n  * has its own --help\n";

	std::cout << m_options << std::endl;

	if (m_positional.max_total_count() > 0)
	{
		std::cout << "Positional Arguments: ";
		for (unsigned int i = 0; i < m_positional.max_total_count(); i++)
		{
			std::cout << "\n  " << i + 1 << ": " << m_positional.name_for_position(i);
		}
		std::cout << std::endl;
	}
}

bool CommandLineParser::runWithoutGUI() const
{
	return m_withoutGUI;
}

bool CommandLineParser::exitApplication() const
{
	return m_quit;
}

bool CommandLineParser::hasError() const
{
	return !m_errorString.empty();
}

std::wstring CommandLineParser::getError()
{
	return m_errorString;
}

void CommandLineParser::processProjectfile()
{
	m_projectFile.makeAbsolute();

	const std::wstring errorstring =
		L"Provided Projectfile is not valid:\n* Provided Projectfile('" + m_projectFile.fileName() +
		L"') ";
	if (!m_projectFile.exists())
	{
		m_errorString = errorstring + L" does not exist";
		m_projectFile = FilePath();
		return;
	}

	if (m_projectFile.extension() != L".srctrlprj")
	{
		m_errorString = errorstring + L" has a wrong file ending";
		m_projectFile = FilePath();
		return;
	}

	std::shared_ptr<ConfigManager> configManager = ConfigManager::createEmpty();
	if (!configManager->load(TextAccess::createFromFile(m_projectFile)))
	{
		m_errorString = errorstring + L" could not be loaded (invalid)";
		m_projectFile = FilePath();
		return;
	}
}

void CommandLineParser::fullRefresh()
{
	m_refreshMode = REFRESH_ALL_FILES;
}

void CommandLineParser::incompleteRefresh()
{
	m_refreshMode = REFRESH_UPDATED_AND_INCOMPLETE_FILES;
}

void CommandLineParser::setShallowIndexingRequested(bool enabled)
{
	m_shallowIndexingRequested = enabled;
}

const FilePath& CommandLineParser::getProjectFilePath() const
{
	return m_projectFile;
}

RefreshMode CommandLineParser::getRefreshMode() const
{
	return m_refreshMode;
}

bool CommandLineParser::getShallowIndexingRequested() const
{
	return m_shallowIndexingRequested;
}

}	 // namespace commandline
