
namespace KEYWORD_TESTS
{

// TEST: certain keywords
// START ----------------------------------------------------------------------

// ACTION: Search for 'overview'
// ACTION: Search for 'errors'
// ACTION: Search for 'legend'
// ACTION: Search for 'file'
// ACTION: Search for 'field'

// RESULTS: correct things are activated

// END ------------------------------------------------------------------------



// TEST: autocomplete within symbol types
// START ----------------------------------------------------------------------

// ACTION: Search for 'file', press tab, enter 'main'
// RESULTS: only 'main.cpp' is shown in list

// END ------------------------------------------------------------------------



// TEST: autocomplete within multiple symbol types
// START ----------------------------------------------------------------------

// ACTION: Search for 'method', press tab, then 'field', enter 'ttt'
// RESULTS: only methods and fields of TicTacToe are in the list

// END ------------------------------------------------------------------------

}

error;
