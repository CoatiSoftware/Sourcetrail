
#define CUSTOM_TRAIL_TESTS

#include "custom_trail_files/custom.h"

// TEST: open and close dialog
// START ----------------------------------------------------------------------

// ACTION 1: Click on custom trail button in graph
// RESULT 1: dialog opens on top of window

// ACTION 2: Close with click on Cancel
// RESULT 2: dialog closes

// ACTION 3: Use custom trail shortcut Ctrl + U
// RESULT 3: dialog opens on top of window

// ACTION 4: click on main window
// ACTION 5: open again
// RESULT 5: dialog opens on top of window

// ACTION 6: press ESC
// RESULT 6: dialog closes

// END ------------------------------------------------------------------------


// TEST: from -> to
// START ----------------------------------------------------------------------

// ACTION 1: search from "CustomD"
// ACTION 2: search to int
// ACTION 3: select infinite depth
// ACTION 4: select horizontal layout
// ACTION 5: select all node and edge types
// Action 6: click search

// RESULTS 6:
// - Graph shows multiple paths
// - Graph contains: CustomD -> CustomB, CustomC -> CustomA, func2 -> func -> int

// Action 7: Repeat search with:
// - select vertical layout
// - uncheck node field

// RESULTS 7:
// - Graph one call chain to func
// - Graph goes top to bottom
// - Graph contains: CustomD -> CustomB -> CustomA -> func -> int

// END ------------------------------------------------------------------------


// TEST: from -> all referenced
// START ----------------------------------------------------------------------

// ACTION 1: search from "CustomD::c"
// ACTION 2: select all referenced
// ACTION 3: select depth 3
// ACTION 4: select horizontal layout
// ACTION 5: select all node and edge types
// Action 6: click search

// RESULTS 6:
// - Graph shows multiple paths
// - Graph contains: CustomD -> CustomC -> int, char, bool, func2, CustomA

// END ------------------------------------------------------------------------


// TEST: from <- all referencing
// START ----------------------------------------------------------------------

// ACTION 1: search from "func"
// ACTION 2: select all referencing
// ACTION 3: select depth infinite
// ACTION 4: select horizontal layout
// ACTION 5: select all node and edge types
// Action 6: click search

// RESULTS 6:
// - Graph shows multiple paths
// - Graph contains: CustomD -> CustomB, CustomC -> CustomA, func2 -> func

// END ------------------------------------------------------------------------
