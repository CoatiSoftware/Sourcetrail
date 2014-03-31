/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ParenPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ParenPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ParenPrinter >( tmp, argc, argv );
    return status;
}
bool suite_UtilityTestSuite_init = false;
#include "D:\projekte\code\2014_master_project\masterproject\src\test\UtilityTestSuite.h"

static UtilityTestSuite suite_UtilityTestSuite;

static CxxTest::List Tests_UtilityTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_UtilityTestSuite( "D:/projekte/code/2014_master_project/masterproject/src/test/UtilityTestSuite.h", 5, "UtilityTestSuite", suite_UtilityTestSuite, Tests_UtilityTestSuite );

static class TestDescription_suite_UtilityTestSuite_test_one_returns_the_number_one : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_UtilityTestSuite_test_one_returns_the_number_one() : CxxTest::RealTestDescription( Tests_UtilityTestSuite, suiteDescription_UtilityTestSuite, 8, "test_one_returns_the_number_one" ) {}
 void runTest() { suite_UtilityTestSuite.test_one_returns_the_number_one(); }
} testDescription_suite_UtilityTestSuite_test_one_returns_the_number_one;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
