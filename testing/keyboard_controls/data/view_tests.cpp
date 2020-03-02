
#define VIEW_TESTS

// TEST: Switch Focus with Tab
// START ----------------------------------------------------------------------

class ViewTestClass {}; // <- ACTION: Activate

// ACTION: Press Tab continuously
// RESULT: Focus is moved between Graph and Code

// END ------------------------------------------------------------------------



// TEST: Focus Search View
// START ----------------------------------------------------------------------

// ACTION: Use Find shortcut
// RESULT: Search view shows focus

// ACTION: press Tab
// RESULT: Search View loses focus

// ACTION: Click into search field
// RESULT: Search view shows focus

// END ------------------------------------------------------------------------



// TEST: Focus Graph View
// START ----------------------------------------------------------------------

// ACTION: Hover nodes using the mouse
// RESULT: graph view receives focus

// END ------------------------------------------------------------------------



// TEST: Focus Code View
// START ----------------------------------------------------------------------

ViewTestClass v; // <- ACTION: Hover location of 'v'

// RESULT: code view receives focus

// END ------------------------------------------------------------------------



// TEST: Graph remembers previously focused node
// START ----------------------------------------------------------------------

// ACTION: Hover a 'ViewTestFocus' in the graph

// ACTION: Press Tab
// RESULT: code view receives focus

// ACTION: Press Tab
// RESULTS:
// - graph receives focus
// - 'ViewTestFocus' is focused again

// END ------------------------------------------------------------------------



// TEST: Code remembers previously focused location
// START ----------------------------------------------------------------------

ViewTestClass var; // <- ACTION: Hover location of 'var'

// ACTION: Press Tab
// RESULT: graph view receives focus

// ACTION: Press Tab
// RESULTS:
// - code receives focus
// - 'var' is focused again

// END ------------------------------------------------------------------------



// TEST: Focus at active symbol when clicked in code
// START ----------------------------------------------------------------------

class ViewTestFocus // <- ACTION: Activate
{
	ViewTestClass member;
};

class ViewTestFocusDerived : public ViewTestFocus {};

// RESULTS:
// - Focus stays in code view
// - Clicked location has focus

// ACTION: Press Tab
// RESULTS:
// - Focus is moved to graph
// - active node receives initial focus

// END ------------------------------------------------------------------------



// TEST: Focus at active symbol when clicked in graph
// START ----------------------------------------------------------------------

class ViewTestFocusUser
{
	ViewTestFocus focus;
};

// ACTION: Activate 'ViewTestFocusUser' in graph

// RESULTS:
// - Focus stays in graph view
// - Clicked node has focus

// ACTION: Press Tab
// RESULTS:
// - Focus is moved to code
// - active location receives initial focus

// END ------------------------------------------------------------------------
