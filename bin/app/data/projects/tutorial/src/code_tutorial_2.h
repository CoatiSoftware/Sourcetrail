#ifndef CODE_TUTORIAL_2_H
#define CODE_TUTORIAL_2_H


#include "code_tutorial_1.h"

//------------------------------------------------------------------------------
//
// MANAGING CONTEXT
//  At this stage you see the whole file. For a shortcut to get here you also
//  could have clicked the maximize button at the title bar of the box that
//  contains this snippet. It is inactive now because the file is already shown
//  as a whole. Before you start experimenting with the other buttons, remember
//  that you can click the maximize buttons to get back here and continue
//  reading.
//  For the final part of this introduction please have a look at where the
//  "unrelated_but_very_important()" function is used.
//
//------------------------------------------------------------------------------


void unrelated_but_very_important()
{
};


int calculate(int (*problem)())
{
	return problem();
}

//------------------------------------------------------------------------------
//
// MORE CONTEXT
//  Who would have expected such a simple question?
//  Note that expanding the view didn't change the active symbol. You can
//  proceed expanding the view until you hit the level of the file scope.
//  So go ahead and find out what else is hidden inside this file!
//
//------------------------------------------------------------------------------

void deep_thought()
{
	int (*the_question)() = [](){ return 6 * 7; };


//------------------------------------------------------------------------------
//
// CONTEXT
//  We just talked about Coati providing context to the important lines of code.
//  But it feels like something is missing here. What is "the_question"? And
//  where does it come from?
//
// EXPANDING TO SCOPES
//  Do you see first line of this snippet? It has no line number and says
//  "deep_thought". Actually this is the name of the scope that encapsulates the
//  snippet. By clicking that line you can tell Coati to expand the view to show
//  the whole scope. Give it a try!
//
//------------------------------------------------------------------------------
	int the_answer = calculate(the_question);
	CodeTutorial::meaning_of_life_the_universe_and_everything = the_answer;
}


#endif // CODE_TUTORIAL_2_H

