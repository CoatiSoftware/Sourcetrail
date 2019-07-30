#ifndef FOO_H
#define FOO_H

#include "FooBar.h"

#ifndef COMPILER_FLAG
	#error "COMPILER_FLAG not defined"
#else

class Bar : public FooBar
{
};

#endif


#endif // FOO_H
