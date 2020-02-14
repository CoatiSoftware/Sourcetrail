#include "catch.hpp"

#include "ApplicationSettings.h"
#include "CommandLineParser.h"

#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("command line")
{
	FilePath appSettingsPath = ApplicationSettings::getInstance()->getFilePath();
	ApplicationSettings::getInstance()->load(FilePath(L"data/CommandlineTestSuite/settings.xml"));

	SECTION("commandline version")
	{
		std::vector<std::string> args({"--version", "help"});

		std::stringstream redStream;
		auto oldBuf = std::cout.rdbuf(redStream.rdbuf());

		commandline::CommandLineParser parser("2016.1");
		parser.preparse(args);
		parser.parse();

		std::cout.rdbuf(oldBuf);

		REQUIRE(redStream.str() == "Sourcetrail Version 2016.1\n");
	}

	SECTION("command config help") {}

	SECTION("command config filepathVector")
	{
		std::vector<std::string> args(
			{"config", "-g", "/usr", "-g", "/usr/share/include", "-g", "/opt/test/include"});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::vector<FilePath> paths = ApplicationSettings::getInstance()->getHeaderSearchPaths();
		REQUIRE(paths[0].wstr() == L"/usr");
		REQUIRE(paths[1].wstr() == L"/usr/share/include");
		REQUIRE(paths[2].wstr() == L"/opt/test/include");
	}

	SECTION("command config string filepath option")
	{
		std::vector<std::string> args({"config", "--maven-path", "/opt/testpath/mvn"});

		std::stringstream redStream;
		auto oldBuf = std::cout.rdbuf(redStream.rdbuf());

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::cout.rdbuf(oldBuf);

		FilePath path = ApplicationSettings::getInstance()->getMavenPath();
		REQUIRE(path.wstr() == L"/opt/testpath/mvn");
	}

	SECTION("command config filepathVector comma separated")
	{
		std::vector<std::string> args(
			{"config", "--global-header-search-paths", "/usr,/usr/include,/include,/opt/include"});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		std::vector<FilePath> paths = ApplicationSettings::getInstance()->getHeaderSearchPaths();
		REQUIRE(paths[0].wstr() == L"/usr");
		REQUIRE(paths[1].wstr() == L"/usr/include");
		REQUIRE(paths[2].wstr() == L"/include");
		REQUIRE(paths[3].wstr() == L"/opt/include");
	}

	SECTION("command config bool options")
	{
		std::vector<std::string> args({"config", "--use-processes", "false"});

		commandline::CommandLineParser parser("2");
		parser.preparse(args);
		parser.parse();

		bool processes = ApplicationSettings::getInstance()->getMultiProcessIndexingEnabled();
		REQUIRE(processes == false);

		std::vector<std::string> args1({"config", "--use-processes", "true"});

		parser.preparse(args1);
		parser.parse();

		processes = ApplicationSettings::getInstance()->getMultiProcessIndexingEnabled();
		REQUIRE(processes == true);
	}

	ApplicationSettings::getInstance()->load(appSettingsPath);
}
