
// TEST: activation loads file - continued
// START ----------------------------------------------------------------------

// RESULT: This file is visible

// END ------------------------------------------------------------------------
































#pragma once

class Reference
{
};


// TEST: reference navigation switches to file - continued
// START ----------------------------------------------------------------------

// RESULT: This file is visible and scrolled here

// ACTION: Use reference navigation at top and navigate to next reference.

// END ------------------------------------------------------------------------

























// TEST: edge click switches to file - continued
// START ----------------------------------------------------------------------

// RESULT: This file is visible and scrolled here

// ACTION: Click on edge from ref_func2() to Reference2

// END ------------------------------------------------------------------------

class Reference2
{
};
