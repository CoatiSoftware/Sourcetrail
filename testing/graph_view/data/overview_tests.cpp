
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



// TEST: overview letter index
// START ----------------------------------------------------------------------

// ACTION 1: activate overview
// ACTION 2: Click on Union bundle
// ACTION 3: Press letter keys on keyboard
// RESULT 3: the graph is scrolled to the respective letter pressed

// END ------------------------------------------------------------------------



union A_Union_Type_Name_Making_A_Long_List {};
union B_Union_Type_Name_Making_A_Long_List {};
union C_Union_Type_Name_Making_A_Long_List {};
union D_Union_Type_Name_Making_A_Long_List {};
union E_Union_Type_Name_Making_A_Long_List {};
union F_Union_Type_Name_Making_A_Long_List {};
union G_Union_Type_Name_Making_A_Long_List {};
union H_Union_Type_Name_Making_A_Long_List {};
union I_Union_Type_Name_Making_A_Long_List {};
union J_Union_Type_Name_Making_A_Long_List {};
union K_Union_Type_Name_Making_A_Long_List {};
union L_Union_Type_Name_Making_A_Long_List {};
union M_Union_Type_Name_Making_A_Long_List {};
union N_Union_Type_Name_Making_A_Long_List {};
union O_Union_Type_Name_Making_A_Long_List {};
union P_Union_Type_Name_Making_A_Long_List {};
union Q_Union_Type_Name_Making_A_Long_List {};
union R_Union_Type_Name_Making_A_Long_List {};
union S_Union_Type_Name_Making_A_Long_List {};
union T_Union_Type_Name_Making_A_Long_List {};
union U_Union_Type_Name_Making_A_Long_List {};
union V_Union_Type_Name_Making_A_Long_List {};
union W_Union_Type_Name_Making_A_Long_List {};
union X_Union_Type_Name_Making_A_Long_List {};
union Y_Union_Type_Name_Making_A_Long_List {};
union Z_Union_Type_Name_Making_A_Long_List {};

int global_variable;
