#include "TestSuiteFixture.h"

#include <sstream>

#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"
#include "utility/logging/PlainFileLogger.h"
#include "settings/ApplicationSettings.h"

TestSuiteFixture::TestSuiteFixture()
{
}

TestSuiteFixture::~TestSuiteFixture()
{
}

#include <iostream>

bool TestSuiteFixture::setUpWorld()
{
	LogManager* logManager = LogManager::getInstance().get();

	logManager->setLoggingEnabled(true);
	logManager->addLogger(std::make_shared<PlainFileLogger>("data/log/test_log.txt"));
	logManager->addLogger(std::make_shared<FileLogger>());

#ifdef __linux__
	const std::string homedir = getenv("HOME");

	if (!homedir.empty())
	{
		if(!ApplicationSettings::getInstance()->load(
			FilePath(homedir + "/.config/coati/ApplicationSettings.xml")
		))
		{
			std::cout << "no settings" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "no homedir" << std::endl;
		return false;
	}

#else
	ApplicationSettings::getInstance()->load(FilePath("data/TestSettings.xml"));
#endif

	return true;
}

bool TestSuiteFixture::tearDownWorld()
{
	LogManager::destroyInstance();

	return true;
}
