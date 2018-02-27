#include "utility/commandline/commands/CommandlineCommandConfig.h"

#include <functional>
#include <utility>

#include "boost/program_options.hpp"

#include "utility/commandline/CommandlineHelper.h"
#include "utility/commandline/CommandLineParser.h"
#include "utility/file/FilePath.h"

#include "settings/ApplicationSettings.h"

namespace commandline {

// helper functions
typedef void(ApplicationSettings::*intFunc)(int);
void parseAndSetValue(
	intFunc f,
	const char* opt,
	ApplicationSettings* settings,
	po::variables_map& vm)
{
	if (vm.count(opt))
	{
		(settings->*f)(vm[opt].as<int>());
	}
}

typedef void(ApplicationSettings::*boolFunc)(bool);
void parseAndSetValue(
	boolFunc f,
	const char* opt,
	ApplicationSettings* settings,
	po::variables_map& vm)
{
	if (vm.count(opt))
	{
		(settings->*f)(vm[opt].as<bool>());
	}
}

typedef void(ApplicationSettings::*filePathFunc)(const FilePath&);
void parseAndSetValue(
	filePathFunc f,
	const char* opt,
	ApplicationSettings* settings,
	po::variables_map& vm)
{
	if (vm.count(opt))
	{
		FilePath path(vm[opt].as<std::string>());
		if (!path.exists())
		{
			std::cout << "\nWARNING: " << path.str()
					  << " does not exist." << std::endl;
		}
		(settings->*f)(path);
	}
}

typedef bool(ApplicationSettings::*vectorFunc)(const std::vector<FilePath>&);
void parseAndSetValue(
	vectorFunc f,
	const char* opt,
	ApplicationSettings* settings,
	po::variables_map& vm)
{
	if (vm.count(opt))
	{
		std::vector<FilePath> v = extractPaths(vm[opt].as<std::vector<std::string>>());
		(settings->*f)(v);
	}
}

CommandConfig::CommandConfig(CommandLineParser* parser)
	: Command("config", parser)
{
}

CommandConfig::~CommandConfig()
{
}

void CommandConfig::setup()
{
	po::options_description options("Config Options");
	options.add_options()
		("help,h", "Print this help message")
		("indexer-threads,t", po::value<int>(), "Set the number of threads used for indexing (0 uses ideal thread count)")
		("use-processes,p", po::value<bool>(), "Enable C/C++ Indexer threads to run in different processes. <true/false>")
		("logging-enabled,l", po::value<bool>(), "Enable file/console logging <true/false>")
		("verbose-indexer-logging-enabled,L", po::value<bool>(),
			"Enable additional log of abstract syntax tree during the indexing. <true/false> WARNINIG Slows down "
			"indexing speed")
		("jvm-path,j", po::value<std::string>(), "Path to the location of the jvm library")
		("jvm-max-memory,M", po::value<int>(),
			"Set the maximum amount of memory for the JVM indexer(-1 for using the JVM default settings)")
		("maven-path,m", po::value<std::string>(), "Path to the maven binary")
		("jre-system-library-paths,J", po::value<std::vector<std::string>>(),
			"paths to the jars of the JRE system library. "
			"These jars can be found inside your JRE install directory (once per path or comma separated)")
		("global-header-search-paths,g", po::value<std::vector<std::string>>(),
			"Global include paths (once per path or comma separated)")
		("global-framework-search-paths,F", po::value<std::vector<std::string>>(),
			"Global include paths (once per path or comma separated)")
		("show,s", "displays all settings")
		;

	m_options.add(options);
}

void printVector(const std::string& title, const std::vector<FilePath>& vec)
{
	std::cout << "\n  " << title << ":";
	if (vec.empty())
	{
		std::cout << "\n    -\n";
	}
	for (const FilePath& item : vec)
	{
		std::cout << "\n    " << item.str();
	}
}

void CommandConfig::printSettings(ApplicationSettings* settings)
{
	std::cout << "Sourcetrail Settings:\n"
			  << "\n  indexer-threads: " << settings->getIndexerThreadCount()
			  << "\n  use-processes: " << settings->getMultiProcessIndexingEnabled()
			  << "\n  logging-enabled: " << settings->getLoggingEnabled()
			  << "\n  verbose-indexer-logging-enabled: " << settings->getVerboseIndexerLoggingEnabled()
			  << "\n  jvm-path: " << settings->getJavaPath().str()
			  << "\n  jvm-max-memory: " << settings->getJavaMaximumMemory()
			  << "\n  maven-path: " << settings->getMavenPath().str();
	printVector("global-header-search-paths", settings->getHeaderSearchPaths());
	printVector("global-framework-search-paths", settings->getFrameworkSearchPaths());
	printVector("jre-system-library-paths", settings->getJreSystemLibraryPaths());
}


ReturnStatus CommandConfig::parse(std::vector<std::string>& args)
{
	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(args).options(m_options).run(), vm);
		po::notify(vm);

		parseConfigFile(vm, m_options);
	}
	catch(po::error& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << m_options << std::endl;
		return ReturnStatus::CMD_FAILURE;
	}

	// when "sourcetrail config" without any options is started,
	// show help since configure nothing wont help
	if (vm.count("help") || args.size() == 0 || args[0] == "help")
	{
		printHelp();
		return ReturnStatus::CMD_QUIT;
	}

	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	if (settings == nullptr)
	{
		LOG_ERROR("No application settings loaded");
		return ReturnStatus::CMD_QUIT;
	}

	if (args[0] == "show" || vm.count("show"))
	{
		printSettings(settings);
		return ReturnStatus::CMD_QUIT;
	}

	parseAndSetValue(&ApplicationSettings::setMultiProcessIndexingEnabled, "use-processes", settings, vm);
	parseAndSetValue(&ApplicationSettings::setLoggingEnabled, "logging-enabled", settings, vm);
	parseAndSetValue(
		&ApplicationSettings::setVerboseIndexerLoggingEnabled,
		"verbose-indexer-logging-enabled",
		settings,
		vm
	);

	parseAndSetValue(&ApplicationSettings::setIndexerThreadCount, "indexer-threads", settings, vm);
	parseAndSetValue(&ApplicationSettings::setJavaMaximumMemory, "jvm-max-memory", settings, vm);

	parseAndSetValue(&ApplicationSettings::setMavenPath, "maven-path", settings, vm);
	parseAndSetValue(&ApplicationSettings::setJavaPath, "jvm-path", settings, vm);

	parseAndSetValue(&ApplicationSettings::setJreSystemLibraryPaths, "jre-system-library-paths", settings, vm);
	parseAndSetValue(&ApplicationSettings::setHeaderSearchPaths, "global-header-search-paths", settings, vm);
	parseAndSetValue(&ApplicationSettings::setFrameworkSearchPaths, "global-framework-search-paths", settings, vm);

	settings->save();

	return ReturnStatus::CMD_QUIT;
}

} // namespace cmd
