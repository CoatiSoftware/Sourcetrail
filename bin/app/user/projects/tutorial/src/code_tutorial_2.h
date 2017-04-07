#ifndef CODE_TUTORIAL_2_H
#define CODE_TUTORIAL_2_H



#include "code_tutorial_1.h"

//------------------------------------------------------------------------------
//
// 5 - NO MORE CONTEXT
//  At this stage you see the entire file and no more context can be expanded.
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
// 4 - MORE CONTEXT
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
// 2 - CONTEXT
//  We just talked about Sourcetrail providing context to the important lines of code.
//  But it feels like something is missing here. What is "the_question"? And
//  where does it come from?
//
// 3 - EXPANDING TO SCOPES
//  Do you see the first line of this snippet? I mean the one that has just two
//  dots as line number and reads "deep_thought". Actually that's the name of
//  the function's scope that encapsulates the snippet. By clicking that line
//  you can tell Sourcetrail to expand the view to show the entire scope.
//  Give it a try!
//
//------------------------------------------------------------------------------
	int the_answer = calculate(the_question);
	CodeTutorial::meaning_of_life_the_universe_and_everything = the_answer;
}



#endif // CODE_TUTORIAL_2_H

