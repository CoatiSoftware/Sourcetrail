#ifndef MY_FIRST_STEP_H
#define MY_FIRST_STEP_H



#include "my_next_step.h"

//------------------------------------------------------------------------------
//
// WHAT JUST HAPPENED?
//  Sourcetrail uses the concept of an "active" symbol. You just changed the active
//	symbol by clicking on the name of a function. 
//
// OK, BUT WHAT IS AN ACTIVE SYMBOL?
//  Sourcetrail doesn't show your entire code base on a single screen. Even for 
//  relatively small projects that would be pointless - trust me on that. So 
//  instead of showing everything at once, Sourcetrail keeps a strongly focused view 
//  on your code. This focus only includes the currently active symbol and its 
//  direct relations. Nothing more. By changing the active symbol as you just 
//  did, Sourcetrail allows you to jump through the code base on your own.
//
// I THINK I GOT IT!
//  Now that you understand Sourcetrail's central concept you are ready to activate 
//  the "my_next_step()" symbol below.
//
//------------------------------------------------------------------------------

void my_first_step()
{
	my_next_step(2);
}



#endif // MY_FIRST_STEP_H

