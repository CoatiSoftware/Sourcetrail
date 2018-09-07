
namespace FULLTEXT_TESTS
{

// TEST: shortcut
// START ----------------------------------------------------------------------

// ACTION: Use Find Shortcut: Edit -> Find Text
// RESULTS:
// - search field is focused
// - content is replaced with '?'
// - followed with cursor

// END ------------------------------------------------------------------------



// TEST: enter button
// START ----------------------------------------------------------------------

// ACTION: Search for '?reset', press enter
// Result: Code view shows 5 references

// END ------------------------------------------------------------------------



// TEST: mouse click
// START ----------------------------------------------------------------------

// ACTION: Search for '?won', press search button
// Result: Code view shows 2 references

// END ------------------------------------------------------------------------



// TEST: implicit
// START ----------------------------------------------------------------------

// ACTION: Search for 'player a', press search button
// Result: fulltext search is used and shows 2 references

// END ------------------------------------------------------------------------



// TEST: case sensitive
// START ----------------------------------------------------------------------

// ACTION: Search for '??game'
// Result: code view shows 3 references

// END ------------------------------------------------------------------------

}
