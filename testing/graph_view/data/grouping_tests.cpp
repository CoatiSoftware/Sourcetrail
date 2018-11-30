
#define GROUPING_TESTS

// TEST: group by namespace
// START ----------------------------------------------------------------------

namespace alpha
{
	void foo() {}
}

namespace beta
{
	void foo() // <- ACTION 1: activate
	{
		alpha::foo();
	}
}

namespace alpha
{
	void bar() { beta::foo(); }
}

// ACTION 2: toggle grouping by namespace on and off
// RESULTS 2:
// - layout changes between grouped not grouped
// - grouped layout has 2 groups: alpha, beta
// - ungrouped layout has 3 nodes

// ACTION 3: click on a group name
// RESULT 3: namespace is activated

// END ------------------------------------------------------------------------



// TEST: group by file
// START ----------------------------------------------------------------------

#include "interaction_files/file.h"
#include "interaction_files/include.h"

int multi_file_function() // <- ACTION 1: activate
{
	File f;
	package::C p;
	Class c;
	c.member = 2;
	Enum e = ENUM_CONSTANT;
}

// ACTION 2: toggle grouping by file on and off
// RESULTS 2:
// - layout changes between grouped not grouped
// - grouped layout has 3 groups: grouping_test.cpp, file.h, include.h
// - ungrouped layout has 5 nodes

// ACTION 3: click on a group name
// RESULT 3: file is activated

// END ------------------------------------------------------------------------
