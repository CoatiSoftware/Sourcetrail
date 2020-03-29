#ifndef GRAPH_TUTORIAL_6_H
#define GRAPH_TUTORIAL_6_H


#include "string.h"

class BoringClass
{
};


//------------------------------------------------------------------------------
//
// 9 - THAT'S IT
//  Great work! You have found this previously hidden base class. Of course you
//  can also use the depth graph controls to expand the all classes that derive
//  from a selected symbol or, if the active symbol is a function or a method it
//  allows you to display the caller or callee graph.
//  You completed the tutorial on the graph view. There is still more to learn
//  but I guess you will figure it out on your own. One last thing: If you see
//  an element with a hatched background please call us. It's a runaway!
//  Just kidding. Those are elements that are used within your code, but have no
//  definition anywhere in your source folder (like the "String" symbol on the
//  right hand side of the graph). So if you click them, Sourcetrail will
//  display all the relations for the element, but the code view won't show a
//	definition. If you want to learn more about Sourcetrail, use the graph to go
//  back to the central hub.
//
//------------------------------------------------------------------------------

class click_me_to_continue
{
	void method(String s)
	{
		the_central_hub();
	}
};

class NothingToSeeHere: public BoringClass, public click_me_to_continue
{
};

class JustMoveOn
{
};


#endif // GRAPH_TUTORIAL_6_H

