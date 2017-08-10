#ifndef GRAPH_TUTORIAL_2_H
#define GRAPH_TUTORIAL_2_H



#include "graph_tutorial_1.h"
#include "graph_tutorial_5.h"

//------------------------------------------------------------------------------
//
// 3 - WELL DONE
//  You activated a node and both, the graph and the code view have been updated
//  accordingly.
//  Note that even though Sourcetrail provides a graph view it does not remove the
//  need for reading code. The graph view is great for gaining a quick overview
//  on relations and navigating code, but once you found something you are
//  interested in you should still consider the code view to actually read up on
//  all the important details.
//
// 4 - HIDDEN ELEMENTS
//  In order not to flood you with information the graph is hiding a lot of
//  elements. Do you see the "ClassWithHiddenMembers" for example? It actually
//  contains more members than you currently see. Members that are not important
//  for the current context are hidden. Nevertheless you can reveal them by 
//  clicking the arrow next the the class name. Try that now and read those 
//  members' names.
//
//------------------------------------------------------------------------------

class ClassWithHiddenMembers
{
public:
	void potential_caller()
	{
		GraphTutorial::where_am_i_called();
	}

	void now_you_see_all_contents_of_this_class()
	{
	}

	void sourcetrail_uses_different_colors()
	{
	}

	void to_distinguish_between_functions()
	{
	}

	int and_variables;

	int to_continue_click;

//------------------------------------------------------------------------------
//
// 5 - RELATIONS
//  As you see in the graph, this member is accessed at quite some places
//  throughout the code base. Lets say you wonder what code causes the edge
//  between "reading" and "this_last_member" to appear. What will happen if you
//  click the edge?
//
//------------------------------------------------------------------------------
	static ClickMeLater this_last_member;
};



#endif // GRAPH_TUTORIAL_2_H

