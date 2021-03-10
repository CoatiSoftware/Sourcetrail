
#define STATE_TESTS

// TEST: edge clicks
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'TicTacToe::Run()'
// ACTION 2: Click edge 'TicTacToe::Run()' -> 'Field::Show()'
// ACTION 3: Click edge 'TicTacToe::Run()' -> 'Player::Turn()'
// ACTION 4: Click 'main()'

// ACTION 5: go back
// RESULT 5: graph and code restored to edge 'TicTacToe::Run()' -> 'Player::Turn()'

// ACTION 6: go back again
// RESULT 6: graph and code restored to edge 'TicTacToe::Run()' -> 'Field::Show()'

// ACTION 6: go back again
// RESULT 6: graph and code restored to node 'TicTacToe::Run()'

// END ------------------------------------------------------------------------



// TEST: graph manipulations & scroll position
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'Field::Show()'
// ACTION 2: Click on 'Field'
// ACTION 3: Expand node 'Player'
// ACTION 4: Hide node 'GameObject'
// ACTION 5: Move node 'numberOut' far to the right
// ACTION 6: Scroll all the way to the right
// ACTION 7: Search for 'main' and activate that symbol

// ACTION 8: go back
// RESULTS 8:
// - scroll position restored
// - graph restored
// - Player expanded
// - GameObject hidden
// - 'numberOut' far right

// ACTION 9: go back
// RESULTS 9:
// - 'GameObject' visible
// - 'numberOut' back to old position
// - Player still expanded

// ACTION 10: go back
// RESULT 10: 'Field::Show()' restored

// END ------------------------------------------------------------------------



// TEST: depth graph
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'io::stringOut'
// ACTION 2: Show caller graph '>'
// ACTION 3: Change depth to 1
// ACTION 3: Hide node 'Field'
// ACTION 4: Search for 'main' and activate that symbol

// ACTION 5: go back
// RESULT 5: graph depth 1 restored

// ACTION 6: go back
// RESULT 6: 'Field' restored

// ACTION 7: go back
// RESULT 7: graph depth 5 restored

// ACTION 8: go back
// RESULT 8: 'io::stringOut' graph restored

// END ------------------------------------------------------------------------



// TEST: graph multiple active
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'diff<..>' and activate that symbol
// ACTION 2: Click on 'diff<int>'
// ACTION 3: Search for 'main' and activate that symbol

// ACTION 4: go back
// RESULT 4: graph to 'diff<int>' restored

// ACTION 5: go back
// RESULT 5: graph with multiple active 'diff<..>' restored

// END ------------------------------------------------------------------------



// TEST: local symbol
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'Field::Show()' and activate that symbol
// ACTION 2: Click local symbol 'row'
// ACTION 3: Activate other symbol

// ACTION 4: go back
// RESULTS 4:
// - graph and code restored to 'Field::Show()'
// - local symbol 'row' active

// END ------------------------------------------------------------------------



// TEST: code reference
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'io::numberIn()'
// ACTION 2: Navigate to 3rd reference
// ACTION 3: Activate other symbol

// ACTION 4: go back
// RESULTS 4: graph and code restored to 3rd reference of 'io::numberIn()'

// END ------------------------------------------------------------------------



// TEST: snippet state & scroll position
// START ----------------------------------------------------------------------

// ACTION 1: Switch to code snippet mode
// ACTION 2: Search for 'io::stringOut()'
// ACTION 3: Minimize 2nd and 3rd snippet
// ACTION 4: Expand last snippet
// ACTION 5: Scroll to recognizable position
// ACTION 6: Activate other symbol

// ACTION 7: go back
// RESULTS 7:
// - Scroll position restored
// - Only first and last snippet expanded

// END ------------------------------------------------------------------------



// TEST: code view mode
// START ----------------------------------------------------------------------

// ACTION 1: Search for 'Field::Show()'
// ACTION 2: Click maximize on second snippet

// ACTION 3: go back
// RESULTS 3:
// - Snippet mode restored
// - Scroll position restored

// END ------------------------------------------------------------------------



// TEST: single mode scroll position
// START ----------------------------------------------------------------------

// ACTION 1: Switch to code single file mode
// ACTION 2: Search for 'Field::SameInRow()'
// ACTION 3: Scroll to recognizable position
// ACTION 4; Activate other symbol

// ACTION 5: go back
// RESULTS 5:
// - Active symbol 'Field::SameInRow()' restored
// - Scroll position restored

// END ------------------------------------------------------------------------
