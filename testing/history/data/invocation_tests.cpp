
#define INVOCATION_TESTS

// TEST: build history
// START ----------------------------------------------------------------------

// ACTIONS: activate all these symbols top to bottom:
class One;
int two;
struct Three;
enum Four
{
	FIVE
};
#define SIX
typedef bool Seven;

// RESULTS:
// - Seven is active
// - dropdown position at top: Seven
// - history menu: Seven at top

// END ------------------------------------------------------------------------



// TEST: dropdown open/close - click
// START ----------------------------------------------------------------------

// ACTION: Expand dropdown and click outside
// RESULT: dropdown disappears

// END ------------------------------------------------------------------------



// TEST: dropdown open/close - ESC
// START ----------------------------------------------------------------------

// ACTION: Expand dropdown and press ESC
// RESULT: dropdown disappears

// END ------------------------------------------------------------------------



// TEST: going back
// START ----------------------------------------------------------------------

// ACTION: Click back button
// ACTION: Use action History -> Back
// ACTION: Context menu: Back
// ACTION: Press Backspace

// RESULTS:
// - symbol name goes down
// - dropdown position goes down
// - history menu: current symbol always on top, end result:
//      Three, Four, FIVE, SIX, Seven, two, One

// END ------------------------------------------------------------------------



// TEST: going forward
// START ----------------------------------------------------------------------

// ACTION: Click forward button
// ACTION: Use action History -> Forward
// ACTION: Context menu: Forward

// RESULTS:
// - symbol name goes up
// - dropdown position goes up
// - history menu: current symbl always on top, end result:
//      SIX, FIVE, Four, Three, Seven, two, One

// END ------------------------------------------------------------------------



// TEST: dropdown selection
// START ----------------------------------------------------------------------

// ACTION: Expand dropdown and click on One
// RESULTS:
// - One is activated
// - dropdown position down at One

// END ------------------------------------------------------------------------



// TEST: history overwrite
// START ----------------------------------------------------------------------

namespace eight {} // <- ACTION: Activate namespace

// RESULTS:
// - dropdown list shrinked to: eight, One
// - dropdown position at top
// - history menu shows: eight, SIX, FIVE, Four, ...

// END ------------------------------------------------------------------------



// TEST: history menu selection
// START ----------------------------------------------------------------------

// ACTION: Select 'Three' from history menu

// RESULTS:
// - dropdown list: Three, eight, One
// - dropdown position at top
// - history menu shows: Three, eight, SIX, FIVE, ...

// END ------------------------------------------------------------------------

