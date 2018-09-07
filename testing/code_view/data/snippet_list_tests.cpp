
namespace SNIPPET_LIST_TESTS
{

// ACTION: switch to snippet mode with button at top right



// TEST: snippets expanded by default
// START ----------------------------------------------------------------------

#include "snippet_list_files/header.h" // <- ACTION: activate file

#include "snippet_list_files/zimpl.h"
#include "snippet_list_files/zimpl2.h"

// END ------------------------------------------------------------------------



// TEST: scope expansion
// START ----------------------------------------------------------------------

class A
{
public:
	int foo()
	{
		m_member = 10;







		int r = 0;

		for (int a = 0; a < 10; a++)
		{
			r += a;
		}

		return r;
	}

private:
	int m_member; // <- ACTION 1: activate member

/*
// RESULTS 1:
// - 2 snippets are visible within the same file
// - this whole comment block is visible

// ACTION 2: expand bottom scope of first snippet: foo()
// RESULT 2: snippets are expanded and merged to one

// ACTION 3: click the class A expansion at the bottom
*/





};

/*
// RESULT 3: the whole class area is visible

// ACTION 4: click the namespace expansion at the top
// RESULT 4: the whole namespace area is visible

// ACTION 5: click the file name expansion at the bottom
// RESULTS 5:
// - the whole file is visible
// - there are not scope expansion lines anymore
*/

// END ------------------------------------------------------------------------



// TEST: file title bar and scroll bar stay fixed
// START ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ACTION: scroll up and down
// RESULT: The file bar at the top and the scrollbar at the bottom stay in view

// END ------------------------------------------------------------------------



// TEST: minimizing snippets
// START ----------------------------------------------------------------------

int a = sum(1, 2); // <- ACTION 1: activate sum

/*
// ACTION 2: click '-' button in snippet bar
// RESULT 2: snippet gets minimized

// ACTION 3: click '=' button in snippet bar
// RESULT 3: snippet expands again

// ACTION 4: click into the file bar area
// RESULT 4: snippet gets minimized

// ACTION 5: click into the file bar area again
// RESULT 5: snippet gets expanded again
*/

// END ------------------------------------------------------------------------



// TEST: maximizing snippets
// START ----------------------------------------------------------------------

int b = sum(3, 4); // <- ACTION 1: activate sum

/*
// ACTION 2: click '[]' button in snippet bar
// RESULT 2: view switches to single file mode

// ACTION 3: click '=' button in snippet bar
// RESULT 3: view switches back to snippet view
*/

// END ------------------------------------------------------------------------



// TEST: reference navigation and scrolling
// START ----------------------------------------------------------------------

int c = sum(5, 6); // <- ACTION 1: activate sum
																					/* RESULT: view is scrolled here -> */ int d = sum(7, 8);

/*
// ACTION 2: Use reference navigation at top back and forward
// RESULTS 2:
// - focus is switched to all locations of sum()
// - if snippet was minimized, it gets expanded
// - view is scrolled to active reference vertically and horizontally
*/

// END ------------------------------------------------------------------------



// TEST: edge click and scrolling
// START ----------------------------------------------------------------------

int e; // <- ACTION 1: activate int

/*
// ACTION 2: In the graph activate the edge between divide & int
// RESULTS 2:
// - snippet is expanded
// - view is scrolled down
// - reference navigation is updated
// - local reference navigation is visible
*/

// END ------------------------------------------------------------------------
}












// END OF FILE
