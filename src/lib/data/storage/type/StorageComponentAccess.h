#ifndef STORAGE_COMPONENT_ACCESS_H
#define STORAGE_COMPONENT_ACCESS_H

#include "utility/types.h"

struct StorageComponentAccess
{
	StorageComponentAccess()
		: nodeId(0)
		, type(0)
	{}

	StorageComponentAccess(Id nodeId, int type)
		: nodeId(nodeId)
		, type(type)
	{}

	Id nodeId;
	int type;
};

#endif // STORAGE_COMPONENT_ACCESS_H
