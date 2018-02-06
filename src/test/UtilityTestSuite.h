#include "cxxtest/TestSuite.h"

#include "utility/utility.h"

class UtilityTestSuite : public CxxTest::TestSuite
{
public:
	void test_trim_blank_spaces_of_string()
	{
		TS_ASSERT_EQUALS(utility::trim(" foo  "), "foo");
	}

	void test_trim_blank_spaces_of_wstring()
	{
		TS_ASSERT_EQUALS(utility::trim(L" foo  "), L"foo");
	}
};
