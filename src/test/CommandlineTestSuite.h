#include "cxxtest/TestSuite.h"

#include "utility/commandline/CommandLineParser.h"
#include "settings/ApplicationSettings.h"

#include <string>
#include <iostream>
#include <sstream>

class CommandlineTestSuite: public CxxTest::TestSuite
{
public:
	void setUp()
	{
		m_appSettingsPath = ApplicationSettings::getInstance()->getFilePath();
		ApplicationSettings::getInstance()->load(FilePath("data/CommandlineTestSuite/settings.xml"));
	}

	void tearDown()
	{
		ApplicationSettings::getInstance()->load(m_appSettingsPath);
	}

	void test_commandline_version()
	{
		std::vector<std::string> args({"--version", "help"});

		std::stringstream redStream;
		auto oldBuf = std::cout.rdbuf( redStream.rdbuf() );

		commandline::CommandLineParser parser("2016.1");
		parser.preparse(args);
		parser.parse();

		std::cout.rdbuf( oldBuf );

		TS_ASSERT_EQUALS(redStream.str(), "Sourcetrail Version 2016.1\n");
	}

	void test_command_config_help()
	{

	}

	void test_command_config_filepathVector()
	{
		std::vector<std::string> args(
		{
			"config",
			"-g",
			"/usr",
			"-g",
			"/usr/share/include",
			"-g",
			"/opt/test/include"
		});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::vector<FilePath> paths = ApplicationSettings::getInstance()->getHeaderSearchPaths();
		TS_ASSERT_EQUALS( paths[0].str(), "/usr")
		TS_ASSERT_EQUALS( paths[1].str(), "/usr/share/include")
		TS_ASSERT_EQUALS( paths[2].str(), "/opt/test/include")
	}

	void test_command_config_string_filepath_option()
	{
		std::vector<std::string> args(
		{
			"config",
			"--maven-path",
			"/opt/testpath/mvn"
		});

		std::stringstream redStream;
		auto oldBuf = std::cout.rdbuf( redStream.rdbuf() );

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::cout.rdbuf( oldBuf );

		FilePath path = ApplicationSettings::getInstance()->getMavenPath();
		TS_ASSERT_EQUALS( path.str(), "/opt/testpath/mvn")

	}

	void test_command_config_filepathVector_comma_separated()
	{
		std::vector<std::string> args(
		{
			"config",
			"--global-header-search-paths",
			"/usr,/usr/include,/include,/opt/include"
		});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::vector<FilePath> paths = ApplicationSettings::getInstance()->getHeaderSearchPaths();
		TS_ASSERT_EQUALS( paths[0].str(), "/usr")
		TS_ASSERT_EQUALS( paths[1].str(), "/usr/include")
		TS_ASSERT_EQUALS( paths[2].str(), "/include")
		TS_ASSERT_EQUALS( paths[3].str(), "/opt/include")

	}

	void test_command_config_bool_options()
	{
		std::vector<std::string> args(
		{
			"config",
			"--use-processes",
			"false"
		});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		bool processes = ApplicationSettings::getInstance()->getMultiProcessIndexingEnabled();
		TS_ASSERT_EQUALS( processes, 0);

		std::vector<std::string> args1(
		{
			"config",
			"--use-processes",
			"true"
		});

		parser.preparse(args1);
		parser.parse();

		processes = ApplicationSettings::getInstance()->getMultiProcessIndexingEnabled();
		TS_ASSERT_EQUALS( processes, 1);
	}

private:
	FilePath m_appSettingsPath;
};
