
namespace LOCAL_REFERENCE_TESTS
{


// TEST: local symbol activation
// START ----------------------------------------------------------------------

void local_symbols()
{
	for (int i = 0; i < 10; i++) // <- ACTION: click on i
	{
		i++;
	}
}

// RESULTS 1:
// - All 4 locations of i are highlighted
// - Each line with a reference shows a line highlight
// - The local reference navigation appears at the top showing "4 local references"

// ACTION 2: <click here>

// RESULTS 2:
// - The local references disappear
// - The local reference navigation disappears

// END ------------------------------------------------------------------------



// TEST: scope start and end brackets activation
// START ----------------------------------------------------------------------

void scope_start_and_end()
{
	for (int i = 0; i < 10; i++)
	{ // <- ACTION 1: click on {
		i++;
	}
} // <- ACTION 2: click on }

// RESULT 1: The start and end curly brackets of the scope are highlighted same as local symbols
// RESULT 2: Same behavior

// END ------------------------------------------------------------------------



// TEST: local reference navigation for local symbols
// START ----------------------------------------------------------------------

void local_reference_navigation()
{
	for (int i = 0; i < 10; i++) // <- ACTION 1: click on i
	{
																							/* RESULT 2: view scrolls here when this reference is active -> */ i--;
		i++;
	}
}

// ACTION 2: use local reference navigation at top to iterate all 5 references back and forward

// RESULTS 2:
// - Each reference is separately highlighted and the view scrolled if necessary
// - The local reference navigation shows "X/5 local references" while navigating

// ACTION 3: Use the menu actions "Edit -> Next Local Reference" and "Edit -> Previous Local Reference" and their shortcuts

// RESULT 3: Same behavior as with buttons

// END ------------------------------------------------------------------------



// TEST: local symbol activation in snippet mode
// START ----------------------------------------------------------------------

void some_referenced_function() { } // <- ACTION 1: activate function

// ACTION 2: switch to snippet mode

void local_reference_navigation_snippet_mode()
{
	some_referenced_function();

	for (int i = 0; i < 10; i++) // <- ACTION 3: click on i
/*
// RESULT 3: 3 local references are active

// ACTION 4: Expand snippet to full scope of function at the snippet bottom
*/








		i++;
}

// RESULT 4: 4 local references are active

// END ------------------------------------------------------------------------



// TEST: local reference navigation for edges
// START ----------------------------------------------------------------------

void some_called_function() {} // <- ACTION 1: activate function

void local_reference_edge_navigation()
{
	some_called_function();

																							/* RESULT: view scrolls here -> */ some_called_function();


	some_called_function();
}

/*
// RESULT 1: 3 local references are active

// ACTION 2: click on the call edge between the two functions in the graph

// RESULTS 2:
// - The references are iterated in forward direction
// - Each reference is separately highlighted and the view scrolled if necessary
// - The local reference navigation shows "X/3 local references" while navigating
*/

// END ------------------------------------------------------------------------

}

