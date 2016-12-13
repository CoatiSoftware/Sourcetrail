#ifndef GRAPH_TUTORIAL_3_H
#define GRAPH_TUTORIAL_3_H



#include "graph_tutorial_2.h"

class ProducerOfTheLastMember
{
public:
	void go()
	{
		ClassWithHiddenMembers::this_last_member = 42;
	}
	
	void on()
	{
		ClassWithHiddenMembers::this_last_member++;
	}
//------------------------------------------------------------------------------
//
// 6 - FOCUSING EDGES
//  Even though Coati doesn't allow to activate an edge, you can still focus on
//  edges. This doesn't change the currently active symbol (which would cause 
//  Coati to hide everything but the edge). Instead Coati just highlights the
//  relation's location in the code and scrolls you there, so you don't lose 
//  context.
//  Try to activate an edge that comes from the "ConsumerOfTheLastMember" class.
//  Which one you pick doesn't really matter for this tutorial.
//
//------------------------------------------------------------------------------
	void reading()
	{
		ClassWithHiddenMembers::this_last_member *= 2;
	}
};



#endif // GRAPH_TUTORIAL_3_H

