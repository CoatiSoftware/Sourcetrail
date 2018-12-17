
namespace ERROR_TESTS
{

// TEST: show errors
// START ----------------------------------------------------------------------

// ACTION: click 'show errors' in the title bar

// RESULTS:
// - title bar shows 3 errors
// - code shows 3 red locations
// - error table is shown at bottom of view with 3 errors
// - search view shows 'error error_tests.cpp'

// END ------------------------------------------------------------------------



// TEST: hover error
// START ----------------------------------------------------------------------

int x = y(); // <- ACTION: hover error locations

// RESULT: tooltip with error message is shown: use of undeclared identifier 'y'

// END ------------------------------------------------------------------------



// TEST: error activation
// START ----------------------------------------------------------------------

class Sample
{
public:
	bool foo() const
	{
		something(); // <- ACTION: click on error
	}

	voi something()
	{
	}
};

// RESULT:
// - Error is highlighted in the error table
// - Reference navigation on top is updated with 2/3 references

// END ------------------------------------------------------------------------



// TEST: error iteration
// START ----------------------------------------------------------------------

// ACTION: use reference navigation on top back and forward

// RESULT: one error after another is activated and scrolled to

// END ------------------------------------------------------------------------

}
