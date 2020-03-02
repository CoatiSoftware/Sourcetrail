
#define GRAPH_TESTS

// TEST: Move Focus between nodes
// START ----------------------------------------------------------------------

class X;
class Y;
class Z;

class A
{
	int a;
	int b;
};

class B : public A // <- ACTION: Activate B
{
	X* x;
	Y* y;
	Z* z;
};

class C : public B
{
	int e;
};

class D
{
	B b;
};

// ACTION: Press Tab
// RESULTS:
// - graph view is focused
// - Node 'B' is focused

// ACTION: Use WASD for navigation
// RESULTS:
// - Focus is moved between nodes
// - Going down from 'B' focuses it's members
// - Going up from 'C' focuses 'B's members

// ACTION: Use HJKL for navigation
// RESULTS: same as above

// ACTION: Use arrow keys for navigation
// RESULTS: same as above

// END ------------------------------------------------------------------------



// TEST: Move Focus between edges
// START ----------------------------------------------------------------------

// ACTION: Hold Shift and use WASD for navigation
// RESULTS:
// - Focus is moved between edges
// - Focus is moved to node if no more edges in that direction

// ACTION: Hold Shift and use HJKL for navigation
// RESULTS: same as above

// ACTION: Hold Shift and use arrow keys for navigation
// RESULTS: same as above

// END ------------------------------------------------------------------------



// TEST: Moving focus beyond viewport centers focus
// START ----------------------------------------------------------------------

// ACTION: Drag classes far apart
// ACTION: Zoom in so not all nodes are visible
// ACTION: Navigate focus with keyboard
// RESULT: View is panned when focused node is outside of the viewport

// END ------------------------------------------------------------------------



// TEST: Expand/collapse nodes
// START ----------------------------------------------------------------------

// ACTION: Move focus on class 'A'
// ACTION: Hold Shift and press Enter/E
// RESULT: 'A' is expanded

// ACTION: Move focus to 'A::a'
// ACTION: Hold Shift and press Enter/E
// RESULTS:
// - 'A' is collapsed
// - 'A' is focused

// END ------------------------------------------------------------------------



// TEST: Activate node
// START ----------------------------------------------------------------------

// ACTION: Move focus on class 'D'
// ACTION: Press Enter/E
// RESULT: 'D' is activated

// END ------------------------------------------------------------------------



// TEST: Activate node in new Tab
// START ----------------------------------------------------------------------

// ACTION: Move focus on 'D::b'
// ACTION: Hold Ctrl/CMD + SHIFT and press Enter/E
// RESULT: 'D::b' is opened in new tab

// END ------------------------------------------------------------------------



// TEST: Activate edge
// START ----------------------------------------------------------------------

B b; // <- ACTION: activate 'B'

// ACTION: Move focus on type use 'b -> B'
// ACTION: Press Enter/E
// RESULT: edge is activated

// END ------------------------------------------------------------------------



// TEST: Activate aggregation edge
// START ----------------------------------------------------------------------

// ACTION: Move focus on aggregation 'D -> B'
// ACTION: Press Enter/E
// RESULT: aggregation edge is activated

// END ------------------------------------------------------------------------



// TEST: Expand bundle node
// START ----------------------------------------------------------------------

// ACTION: Move focus on 'Non-indexed Symbols' bundle
// ACTION: Press Enter/E
// RESULTS:
// - bundle is split
// - focus is moved to active symbol

// END ------------------------------------------------------------------------
