#ifndef CODE_TUTORIAL_3_H
#define CODE_TUTORIAL_3_H


#include "the_central_hub.h"
#include "code_tutorial_2.h"

//------------------------------------------------------------------------------
//
// SNIPPETS AND FILES
//  It looks like this function is called in two different snippets. When two
//  different snippets are located in the same file they share a common file
//  view but each of them depicts the name of its own parent's scope.
//  If you want to show the lines in between these two snippets you can either
//  expand the upper snippet's scope to show the whole file or you can tell the
//  lower snippet to reveal its scope.
//  Try one of these approaches now.
//
//------------------------------------------------------------------------------

void function_with_snippets()
{
	unrelated_but_very_important();


//------------------------------------------------------------------------------
//
// YOU FOUND THE HIDDEN COMMENT
//  Well done! As you see Coati merged the two snippets from before because
//  expanding a scope would have caused them to overlap. That's all for the code
//  view.
//
// EDIT YOUR CODE
//  In case you wondered if you can edit your code using Coati: You can't. Coati
//  is designed for code exploration, not as a text editor or an IDE. But there
//  is a chance that Coati can talk to your favourite editor (that you already
//  know how to operate) via plugin. For more information about available
//  plugins please take a look at the Coati User Manual.
//
//  We hope you enjoyed this tutorial. You can follow the call below to get back
//  to the central hub.
//
// P.S.
//  You can also click the file name above to activate the file's node in case
//  you want to explore your include hierarchy.
//
//------------------------------------------------------------------------------

	the_central_hub();


	unrelated_but_very_important();
};


#endif // CODE_TUTORIAL_3_H

