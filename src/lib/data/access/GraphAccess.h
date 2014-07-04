#ifndef GRAPH_ACCESS_H
#define GRAPH_ACCESS_H

#include <string>

#include "utility/types.h"

class Token;

class GraphAccess
{
public:
	virtual ~GraphAccess();

	virtual Id getIdForNodeWithName(const std::string& name) const = 0;
	virtual std::string getNameForNodeWithId(Id id) const = 0;
};


#endif // GRAPH_ACCESS_H
