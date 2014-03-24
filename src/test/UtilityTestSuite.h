#include <cxxtest/TestSuite.h>

#include "utility.h"

class UtilityTestSuite: public CxxTest::TestSuite
{
public:
	void test_one_returns_the_number_one(void)
	{
		TS_ASSERT_EQUALS(one(), 1);
	}
};
