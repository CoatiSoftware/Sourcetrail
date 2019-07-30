#ifndef BAR_H
#define BAR_H

#include "FooBar.h"

#ifndef CDB_FLAG
	#error "CDB_FLAG not defined"
#endif

class Foo : public FooBar
{
};

#endif // FOO_BAR_H
