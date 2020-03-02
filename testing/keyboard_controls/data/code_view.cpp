
#define CODE_TESTS

#include "header.h"

// NOTE: complete in both snippet and single mode

// TEST: Move Focus between locations
// START ----------------------------------------------------------------------

int sum(int a, int b) // <- ACTION: focus location via hover
{
	return a + b;
}

int diff(int a, int b)
{
	return a - b;
}

int mult(int a, int b)
{
	return a * b;
}

int div(int a, int b)
{
	return a / b;
}

// ACTION: Use WASD for navigation
// RESULTS:
// - Focus is moved between locations
// - lines without locations are skipped
// - focus tries to stay at same column if possible
// - view scrolls up and down if focus leaves visible viewport

// ACTION: Use HJKL for navigation
// RESULTS: same as above

// ACTION: Use arrow keys for navigation
// RESULTS: same as above

// END ------------------------------------------------------------------------



// TEST: Activate location
// START ----------------------------------------------------------------------

int decimate(int a) // <- ACTION: move focus on 'decimate'
{
	return div(a, 10);
}

// ACTION: Press Enter/E
// RESULT: 'decimate' is activated

// END ------------------------------------------------------------------------



// TEST: Activate local symbol
// START ----------------------------------------------------------------------

int pow(int a, int b)
{
	int r = 1; // <- ACTION 1: move focus on 'r'
	for (int i = 0; i < b; ++i)
	{
		r *= a;
	}
	return r; // <- ACTION 3: move focus on 'r'
}

// ACTION 2: Press Enter/E
// RESULT 2: 'r' is activated

// ACTION 4: Press Enter/E
// RESULT 3: 'r' is deactivated

// END ------------------------------------------------------------------------



// TEST: Navigate between active locations
// START ----------------------------------------------------------------------

int square(int a)
{
	return mult(a, a); // <- ACTION: activate 'mult'
}

// ACTION: Hold Shift and navigate with keyboard
// RESULT: focus is moved between the active locations of 'mult'

// END ------------------------------------------------------------------------


// TEST: Navigate between active local locations
// START ----------------------------------------------------------------------

int cube(int a)
{
	int r = square(a); // <- ACTION 1: activate 'square'
	r = mult(r, a);
	return r; // <- ACTION 2: activate 'r'
}

// ACTION: Hold Shift and navigate with keyboard
// RESULTS:
// - focus is moved between the active locations of 'square'
// - focus is moved between the active local locations of 'r' inbetween

// END ------------------------------------------------------------------------



// NOTE: Snippet mode only



// TEST: Expanding scopes
// START ----------------------------------------------------------------------

// ACTION 5: Move focus to file expansion above and press Enter/E

class Calculator
{
public:
	int getValue() const
	{
		return m_value;
	}

	// ACTION 4: Move focus to scope expansion above and press Enter/E
	void clearValue()
	{
		m_value = 0;
	}

	void add(int a) // <- ACTION 1: activate 'add'
	{
		m_value += a;
	}
	// ACTION 2: Move focus to scope expansion below and press Enter/E

	void diff(int a)
	{
		m_value -= a;
	}
	// ACTION 3: Move focus to scope expansion above and press Enter/E

	void mult(int a)
	{
		m_value *= a;
	}

	void div(int a)
	{
		m_value /= a;
	}

private:
	int m_value = 0;
};

// RESULTS 1-4:
// - scope is expanded and focus is placed to location closest to expanded line

// BROKEN: focus is placed but not visible
// RESULT 5: Whole file is expanded and focus placed at closest location

// END ------------------------------------------------------------------------



// TEST: Expand/Minimize snippets
// START ----------------------------------------------------------------------

int half(int a) // <- ACTION 2: activate 'int'
{
	return div(a, 2);
}

// ACTION 2: Move focus to snippet title bar and press Enter/E
// RESULT 2: Snippet is collapsed

// ACTION 2: Press Enter/E again
// RESULT 2: Snippet is expanded again

// END ------------------------------------------------------------------------
