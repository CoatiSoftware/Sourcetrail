#ifndef GRAPH_TUTORIAL_4_H
#define GRAPH_TUTORIAL_4_H



#include "graph_tutorial_2.h"
#include "the_central_hub.h"

class ConsumerOfTheLastMember
{
public:
	void go()
	{
		ClassWithHiddenMembers::this_last_member.consume();
	}

	void on()
	{
		ClassWithHiddenMembers::this_last_member.consume();
	}
//------------------------------------------------------------------------------
//
// 6 - FOCUSING EDGES
//  Even though Sourcetrail doesn't allow to activate an edge, you can still focus on
//  edges. This doesn't change the currently active symbol (which would cause
//  Sourcetrail to hide everything but the edge). Instead Sourcetrail just highlights the
//  relation's location in the code and scrolls you there, so you don't lose
//  context.
//  Try to activate an edge that comes from the "ProducerOfTheLastMember" class.
//  Which one you pick doesn't really matter for this tutorial.
//
//------------------------------------------------------------------------------
	void reading()
	{
		ClassWithHiddenMembers::this_last_member.consume();
	}
};



#endif // GRAPH_TUTORIAL_4_H

