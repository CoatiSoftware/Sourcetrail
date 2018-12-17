#include <cmath>
#include <string>
#include <vector>

namespace CODE_AREA_TESTS
{

// TEST: hover
// START ----------------------------------------------------------------------

namespace hover // <- ACTION 1: hover namespace
{
	void function(size_t number, bool flag) // <- ACTION 2: function
	{
		if (flag)
		{
			number += 10; // <- ACTION 3: hover number
		}
	}
}

// RESULTS 1:
// - namespace name gets border
// - namespace and it's scope show highlight at line numbers
// - tooltip shows CODE_AREA_TESTS::hover

// RESULTS 2:
// - function name gets border
// - function and it's scope show highlight at line numbers
// - tooltip shows function signature

// RESULTS 3:
// - variable name and all references get highlighted
// - every line with a reference shows a highlight at the line number
// - no tooltip is shown

// END ------------------------------------------------------------------------



// TEST: Activating symbol
// START ----------------------------------------------------------------------

void some_function() // <- ACTION: click on function
{
}

// RESULTS:
// - function is activated
// - reference navigation at top shows "1 reference"

// END ------------------------------------------------------------------------



// TEST: Activating reference
// START ----------------------------------------------------------------------

void func() {}

void reference_test()
{
	func(); // <- ACTION: click on function call
}

// RESULTS:
// - function is activated
// - definition location and reference location is active
// - reference navigation at top shows "2 references"

// END ------------------------------------------------------------------------



// TEST: Activating qualifier
// START ----------------------------------------------------------------------

namespace qualifier
{
	void func() {}
}

void qualifier_test()
{
	/* ACTION: Click on qualifier -> */ qualifier::func();
}

// RESULTS:
// - qualifier is activated
// - clicked qualifier location is not highlighted
// - reference navigation shows "1 reference"

// END ------------------------------------------------------------------------



// TEST: members have implicit reference locations at type definition
// START ----------------------------------------------------------------------
class TestType
{};

class Sample // <- ACTION 1: click class name
{
public:
	TestType m_type;
};

Sample s;

// RESULT 1: shows list:
// Sample
// Sample::m_type

// ACTION 2: click on Sample::m_type

// RESULT 2: member has 2 references, one at class name

// END ------------------------------------------------------------------------



// TEST: clicking on overriden method activates it immediately
// START ----------------------------------------------------------------------
class Base
{
	virtual void foo();
};

class Override
	: public Base
{
	void foo() override; // <- ACTION: click on foo()
};

// RESULT: Override::foo() is activated

// END ------------------------------------------------------------------------


// TEST: clicking on macro usage shows list of symbols referenced at location
// START ----------------------------------------------------------------------

#define POW(__number__, __power__) \
	std::pow(__number__, __power__)

int a = POW(42, 2); // <- ACTION 1: click on POW

// RESULT 1: shows list:
// POW
// pow<int, int>
// std

// ACTION 2: click on POW in the list

// RESULT 2: POW is activated

// END ------------------------------------------------------------------------



// TEST: function tooltip - BROKEN
// START ----------------------------------------------------------------------

std::vector<std::string> very_long_function_name_with_three_parameters( // <- ACTION 1: hover function
	const std::vector<std::string>& strings, size_t number, bool flag)
{
	return strings;
}

// RESULT 1:
// - Tooltip shows signature as defined in code with qualifier:
// std::vector<std::string>
//  CODE_AREA_TESTS::very_long_function_name_with_three_parameters(
//  	const std::vector<std::string>& strings,
//  	size_t number,
//  	bool flag
//  )

// ACTION 2: hover all symbols in the tooltip

// RESULT 2: all types, qualifier and function is clickable

// ACTION 3: click on one of them

// RESULT 3: Clicked symbol is activated

// END ------------------------------------------------------------------------



// TEST: auto tooltip - BROKEN
// START ----------------------------------------------------------------------

void auto_function(const std::vector<std::string>& strings)
{
	auto it = strings.begin(); // <- ACTION 1: hover auto
}

// RESULT 1: Tooltip shows underlying type

// END ------------------------------------------------------------------------



// TEST: typedef tooltip - BROKEN
// START ----------------------------------------------------------------------

typedef unsigned int uint;

uint b = 12; // <- ACTION 1: hover uint

// RESULT 1: Tooltip shows underlying type

// END ------------------------------------------------------------------------



// TEST: horizontal panning
// START ----------------------------------------------------------------------

void very_long_function_name_with_three_parameters_that_is_so_long_it_doesnt_fit_on_the_screen(const std::vector<std::string>& strings, size_t number, bool flag);

// ACTION 1: Hold SHIFT and drag code left and right

// RESULT 1: It pans left and right

// ACTION 2: Switch to snippet/single mode and repeat

// RESULT 2: same thing

// END ------------------------------------------------------------------------



// TEST: copy & paste
// START ----------------------------------------------------------------------

void copy_function(
	const std::vector<std::string>& strings,
	size_t number,
	bool flag)
{
	for (auto str : strings)
	{
		number++;
	}
}

// ^ ACTION: Select whole piece of code and paste it into a text editor

// RESULT: All code is pasted.

// END ------------------------------------------------------------------------



// TEST: copy file path
// START ----------------------------------------------------------------------

// ACTION: Open context menu and choose "Copy Full Path" and paste it to a text editor.

// RESULT: Full path to the file is pasted

// END ------------------------------------------------------------------------



// TEST: open containing folder
// START ----------------------------------------------------------------------

// ACTION: Open context menu and choose "Open Containing Folder".

// RESULT: The filesystem explorer is opened showing the "data" directory of this testsuite.

// END ------------------------------------------------------------------------

}
