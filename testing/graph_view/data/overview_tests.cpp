
#define OVERVIEW_TESTS


// TEST: activate overview
// START ----------------------------------------------------------------------

// ACTION 1: click on overview button
// RESULT 1: overview is activated

// ACTION 2: go back and use menu action "Edit -> To overview"
// RESULT 2: overview is activated

// END ------------------------------------------------------------------------



// TEST: overview bundles
// START ----------------------------------------------------------------------

// ACTION 1: activate overview
// RESULTS 1:
// - overview shows 10 bundles
// - Files, Macros, Namespaces, Classes, Structs, Unions, functions, global_variable, Typedefs, Enums

// END ------------------------------------------------------------------------



// TEST: overview bundle split
// START ----------------------------------------------------------------------

// ACTION 1: activate overview
// ACTION 2: Click on Classes bundle
// RESULTS 2:
// - all classes are displayed within a group frame
// - the nodes are alphabetically sorted by starting letter
// - the group frame does not react to clicks

// ACTION 3: Click on a class
// RESULT 3: the class is activated

// END ------------------------------------------------------------------------


union A_Union_Type {};
int global_variable;
