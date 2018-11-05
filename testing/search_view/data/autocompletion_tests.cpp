
namespace AUTOCOMPLETION_TESTS
{

// TEST: Find Shortcut
// START ----------------------------------------------------------------------

// ACTION: Use Find Shortcut: Edit -> Find Symbol
// RESULTS:
// - search field is focused
// - content is selected
// - when typing content is cleared

// END ------------------------------------------------------------------------



// TEST: Find Symbol
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'main' and press enter
// RESULT: main is activated

// END ------------------------------------------------------------------------



// TEST: Find Symbol with button
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'main' and click find button
// RESULT: main is activated

// END ------------------------------------------------------------------------



// TEST: Choose symbol from list - keyboard
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'main' and use down arrow to select second symbol, press enter
// RESULT: 'main.cpp' is activated

// END ------------------------------------------------------------------------



// TEST: Choose symbol from list - mouse
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'main' and click in list on 3rd symbol
// RESULT: 'HumanPlayer::Input' is activated

// END ------------------------------------------------------------------------



// TEST: Fuzzy matching
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'ttt'
// RESULT: top result is class 'TicTacToe'

// END ------------------------------------------------------------------------



// TEST: Tab completion to find symbols
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'ttt' and press tab
// RESULTS:
// - content is extended to 'TicTacToe::'
// - all children of 'TicTacToe' are shown

// END ------------------------------------------------------------------------



// TEST: Namespace as subtext
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'num'
// RESULT: top result is 'io::numberIn', namespace 'io' in subtext

// END ------------------------------------------------------------------------



// TEST: Directory as subtext
// START ----------------------------------------------------------------------

// ACTION: Search symbol 'ttt.'
// RESULT: top result is 'tictactoe.h', directory in subtext

// END ------------------------------------------------------------------------



// TEST: No implicit template specializations in autocompletions
// START ----------------------------------------------------------------------

// ACTION 1: Search symbol 'sum'
// RESULT 1: List shows 'sum<int>' and 'sum<typename T>, but not 'sum<double>'

// ACTION 2: Activate 'sum<typename T>'
// RESULT 2: Graph shows 'sum<double>' and 'sum<unsigned int>' as well

// END ------------------------------------------------------------------------



// TEST: Undefined templates shortened with '<..>'
// START ----------------------------------------------------------------------

// ACTION 1: Search symbol 'diff'
// RESULT 1: List shows 'diff<..>'

// ACTION 2: Activate 'diff<..>'
// RESULT 2: Graph shows 'diff<double>' and 'diff<int>'

// END ------------------------------------------------------------------------

}
