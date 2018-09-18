#include "TestSuiteFixture.h"

#include <iostream>

#include "ApplicationSettings.h"

TestSuiteFixture::TestSuiteFixture()
{
}

TestSuiteFixture::~TestSuiteFixture()
{
}

bool TestSuiteFixture::setUpWorld()
{
#ifdef __linux__
	const std::string homedir = getenv("HOME");

	if (!homedir.empty())
	{
		if(!ApplicationSettings::getInstance()->load(
			FilePath(homedir + "/.config/sourcetrail/ApplicationSettings.xml")
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
	ApplicationSettings::getInstance()->load(FilePath(L"data/TestSettings.xml"));
#endif

	return true;
}

bool TestSuiteFixture::tearDownWorld()
{
	return true;
}
