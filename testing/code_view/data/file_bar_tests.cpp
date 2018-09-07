
namespace FILE_BAR_TESTS
{

// TEST: file button tooltip
// START ----------------------------------------------------------------------

// ACTION: Hover file name in the title bar

// RESULT: The tooltip shows the whole path

// END ------------------------------------------------------------------------



// TEST: file changed indicator
// START ----------------------------------------------------------------------

// ACTION 1: Make changes to this file, then switch back here

// RESULT 1: The file name has an * added at the end.

// ACTION 2: Hover file name

// RESULT 2: Tooltip also shows "out-of-data file:"

// END ------------------------------------------------------------------------



// TEST: non-indexed file
// START ----------------------------------------------------------------------

#include "file_bar_files/non_indexed.h" // <- ACTION 1: activate file

// END ------------------------------------------------------------------------



// TEST: incomplete file
// START ----------------------------------------------------------------------

#include "file_bar_files/incomplete.h" // <- ACTION: activate file

// END ------------------------------------------------------------------------



// TEST: non-indexed & incomplete file
// START ----------------------------------------------------------------------

#include "file_bar_files/non_indexed_and_incomplete.h" // <- ACTION: activate file

// END ------------------------------------------------------------------------



// TEST: edit project in overview
// START ----------------------------------------------------------------------

// ACTION 1: go to overview and hover project name

// RESULT 1: File button has pen icon and tooltip shows 'edit project'

// ACTION 2: click project name in code file bar

// RESULT 2: Edit project dialog is shown

// END ------------------------------------------------------------------------

}
