#include "TestSuiteFixture.h"

#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/logging.h"
#include "utility/logging/LogManager.h"

TestSuiteFixture::TestSuiteFixture()
{
}

TestSuiteFixture::~TestSuiteFixture()
{
}

bool TestSuiteFixture::setUp()
{
	LogManager::getInstance()->addLogger(std::make_shared<ConsoleLogger>());
	LogManager::getInstance()->addLogger(std::make_shared<FileLogger>());

	return true;
}

bool TestSuiteFixture::tearDown()
{
	LogManager::destroyInstance();

	return true;
}
