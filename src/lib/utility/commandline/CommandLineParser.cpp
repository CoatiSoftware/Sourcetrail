#include "utility/commandline/CommandLineParser.h"

#include "boost/program_options.hpp"

#include "utility/ConfigManager.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"
#include "License.h"
#include "PublicKey.h"

namespace po = boost::program_options;

CommandLineParser::CommandLineParser(int argc, char** argv, const std::string& version)
	: m_force(false)
	, m_quit(false)
	, m_withLicense(false)
	, m_withoutGUI(false)
{
	std::string projectfile;
	std::string projectfile_db;
	std::string licensefile;
	std::string licensetext;
	po::options_description desc("Coati");
	desc.add_options()
		("help,h", "Print this help message")
		("version,v", "Version of Coati")
		("project,p", "Load Coati with a Coatiprojectfile")
		;
	po::positional_options_description positionalOption;
	positionalOption.add("project-file", 1);

	po::options_description hidden_desc("hidden commandlineflags");
	hidden_desc.add_options()
		("force,f", "Force coati to parse if database exists")
		("licenseFile,z", po::value<std::string>(&licensefile), "Enter license via Licensefile")
		("license,l", po::value<std::string>(&licensetext), "Enter licenes via commandline")
		("hidden", "Print this help message with hidden arguments")
		("database,d", "Start coati to parse a Coati projectfile to get a coatidatabase")
		("project-file", po::value<std::string>(&projectfile), "Coati project file");
		;
	po::options_description all("Allowed options");
	all.add(desc).add(hidden_desc);

	po::variables_map vm;
	po::store(po::command_line_parser(argc,argv).options(all).positional(positionalOption).allow_unregistered().run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		m_quit = true;
	}

	if (vm.count("hidden"))
	{
		std::cout << all << std::endl;
		m_quit = true;
	}

	if (vm.count("force"))
	{
		m_force = true;
	}

	if (vm.count("license"))
	{
		licensetext = utility::replace(licensetext, "\\n", "\n");
		processLicense(m_license.loadFromString(licensetext));
	}

	if (vm.count("licenseFile"))
	{
		std::cout << "licensefile flag" << std::endl;
		if (FileSystem::exists(licensefile))
		{
			std::cout << "licensefile exists" << std::endl;
			processLicense(m_license.loadFromFile(licensefile));
		}
		else
		{
			std::cout << licensefile << " not found" << std::endl;
		}
	}

	if (vm.count("version"))
	{
		std::cout << "Coati Version " << version << std::endl;
		m_quit = true;
	}

	if (vm.count("project-file"))
	{
		processProjectfile(projectfile);
		if (vm.count("database"))
		{
			m_withoutGUI = true;
		}
	}
	else if (vm.count("database") || vm.count("project") ) {
		std::cout << "A project file is needed for this option" << std::endl;
	}
}

void CommandLineParser::processLicense(const bool isLoaded)
{
	if (!isLoaded)
	{
		std::cout << "Could not load License" << std::endl;
	}
	m_license.loadPublicKeyFromString(PublicKey);
	if (!m_license.isValid())
	{
		std::cout << "License is not valid" << std::endl;
	}
	m_withLicense = true;
	m_withoutGUI = true;
}

CommandLineParser::~CommandLineParser()
{
}

bool CommandLineParser::runWithoutGUI()
{
	return m_withoutGUI;
}

bool CommandLineParser::exitApplication()
{
	return m_quit;
}

bool CommandLineParser::startedWithLicense()
{
	return m_withLicense;
}

void CommandLineParser::processProjectfile(const std::string& file)
{
	FilePath projectfile(file);
	bool isValidProjectfile = true;
	std::string errorstring = "Provided Projectfile is not valid:\n";
	std::string errorProjectfile = "\tProvided Projectfile('" + projectfile.fileName() + ") ";
	if (!projectfile.exists())
	{
		errorstring += errorProjectfile + " does not exist\n";
		isValidProjectfile = false;
	}

	if (projectfile.extension() != ".coatiproject")
	{
		errorstring += errorProjectfile + " has a wrong fileending\n";
		isValidProjectfile = false;
	}

	std::shared_ptr<ConfigManager> configManager = ConfigManager::createEmpty();
	if (!configManager->load(TextAccess::createFromFile(projectfile.str())))
	{
		errorstring += errorProjectfile + " could not be loaded\n";
		isValidProjectfile = false;
	}

	if (isValidProjectfile)
	{
		m_projectFile = projectfile;
	}
	else
	{
		std::cout << errorstring << std::endl;
	}
}

void CommandLineParser::projectLoad()
{
	if (m_projectFile.exists() && m_projectFile.extension() == ".coatiproject")
	{
		MessageDispatchWhenLicenseValid(
			std::make_shared<MessageLoadProject>(m_projectFile.str(), m_force)
		).dispatch();
	}
}

License CommandLineParser::getLicense()
{
	return m_license;
}

