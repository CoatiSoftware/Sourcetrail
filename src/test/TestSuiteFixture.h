#ifndef TEST_SUITE_FIXTURE_H
#define TEST_SUITE_FIXTURE_H

#include "cxxtest/GlobalFixture.h"

class TestSuiteFixture : public CxxTest::GlobalFixture
{
public:
	TestSuiteFixture();
	virtual ~TestSuiteFixture();

	virtual bool setUpWorld();
	virtual bool tearDownWorld();
};

// According to the CxxTest Documentation global fixtures are actually supposed to be implemented as global static instances
// See http://cxxtest.com/guide.html for more details
static TestSuiteFixture testSuiteFixture;

#endif // TEST_SUITE_FIXTURE_H
