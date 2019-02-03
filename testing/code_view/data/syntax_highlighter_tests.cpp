
namespace SYNTAX_HIGHLIGHTER_TESTS
{

// TEST: multiline comments
// START ----------------------------------------------------------------------

/**/

//*

///*

// /* /*

//* /*

/* comment */

/* comment *//* comment */

/* comment */ /* comment */

/* comment /* inside comment */

/* comment
 * comment
 * comment
 */

/* start
   comment
// end */

/*/
	comment
/*/

/* comment */ int no_comment; /* comment */

int no_comment2; /*
 					* comment
 					* comment
 									*/ int no_comment3;

// /*
int no_comment4;
// */

//*
int no_comment5;
//*

const char no_commentStr1[] = " /* string */ "/* comment */;
const char no_commentStr2[] = " /* "; /* comment */
const char no_commentStr3[] = /* const char[] a = " no string */" this is a string "; // <- highlight broken

// RESULT: All no_comment variable are not highlighted as comment

// END ------------------------------------------------------------------------

}
