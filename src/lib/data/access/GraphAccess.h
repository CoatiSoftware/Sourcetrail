#ifndef GRAPH_ACCESS_H
#define GRAPH_ACCESS_H

#include "utility/types.h"

class Token;

class GraphAccess
{
public:
	virtual ~GraphAccess();

	virtual Token* getToken(Id tokenId) const = 0;
};


#endif // GRAPH_ACCESS_H
