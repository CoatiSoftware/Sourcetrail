#ifndef CODE_TUTORIAL_3_H
#define CODE_TUTORIAL_3_H



#include "the_central_hub.h"
#include "code_tutorial_2.h"

//------------------------------------------------------------------------------
//
// 6 - SNIPPETS AND FILES
//  It looks like this function is called in two different places inside the
//  same file. When two different snippets are located in the same file they
//  share a single file box. The line numbers to the left indicate where each of
//  these snippets is located inside the file.
//
// 7 - MERGING SNIPPETS
//  The top line of each snippet shows the name of its own parent's scope.
//  If you want to show the lines in between the two snippets below you can
//  either expand the upper snippet's scope to show the whole file or you can
//  tell the lower snippet to reveal its scope.
//  Try one of these approaches now.
//
//------------------------------------------------------------------------------

void function_with_snippets()
{
	unrelated_but_very_important();



//------------------------------------------------------------------------------
//
// 8 - YOU FOUND THE HIDDEN COMMENT
//  Well done! As you see Sourcetrail merged the two snippets from before because
//  expanding a scope would have caused them to overlap. That's all for the code
//  view tutorial.
//
// 9 - EDIT YOUR CODE
//  In case you wondered if you can edit your code using Sourcetrail: You can't. Sourcetrail
//  is designed for code exploration, not as a text editor or an IDE. But there
//  is a chance that Sourcetrail can talk to your favorite editor (that you already
//  know how to operate) via plugin. For more information about available
//  plugins please take a look at Sourcetrail's documentation.
//
//  We hope you enjoyed this tutorial. You can follow the call below to get back
//  to the central hub.
//
// P.S.
//  You can also click the file name above to activate the file's node in case you
//  want to explore your include hierarchy.
//
//------------------------------------------------------------------------------


	the_central_hub();



	unrelated_but_very_important();
};



#endif // CODE_TUTORIAL_3_H

