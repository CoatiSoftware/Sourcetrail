#include "TestSuiteFixture.h"

#include <sstream>

#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"
#include "utility/logging/PlainFileLogger.h"

TestSuiteFixture::TestSuiteFixture()
{
}

TestSuiteFixture::~TestSuiteFixture()
{
}

bool TestSuiteFixture::setUpWorld()
{
	LogManager::getInstance()->addLogger(std::make_shared<PlainFileLogger>("data/log/test_log.txt"));
	LogManager::getInstance()->addLogger(std::make_shared<FileLogger>());

	return true;
}

bool TestSuiteFixture::tearDownWorld()
{
	LogManager::destroyInstance();

	return true;
}
