
namespace SINGLE_FILE_TESTS
{

// ACTION: switch to single file mode with button at top right


// TEST: activation loads file
// START ----------------------------------------------------------------------

#include "single_file_files/header.h" // <- ACTION: activate file

// END ------------------------------------------------------------------------



// TEST: reference navigation switches to file
// START ----------------------------------------------------------------------

#include "single_file_files/reference.h"

Reference r; // <- ACTION: activate class

// END ------------------------------------------------------------------------



// TEST: edge click switches to file
// START ----------------------------------------------------------------------

#include "single_file_files/reference2.h"

Reference2 r2; // <- ACTION: activate class

// END ------------------------------------------------------------------------
}
