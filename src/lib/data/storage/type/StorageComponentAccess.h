#ifndef STORAGE_COMPONENT_ACCESS_H
#define STORAGE_COMPONENT_ACCESS_H

#include "utility/types.h"

struct StorageComponentAccessData
{
	StorageComponentAccessData()
		: nodeId(0)
		, type(0)
	{}

	StorageComponentAccessData(Id nodeId, int type)
		: nodeId(nodeId)
		, type(type)
	{}

	Id nodeId;
	int type;
};

struct StorageComponentAccess: public StorageComponentAccessData
{
	StorageComponentAccess()
		: StorageComponentAccessData()
		, id(0)
	{}

	StorageComponentAccess(Id id, const StorageComponentAccessData data)
		: StorageComponentAccessData(data)
		, id(nodeId)
	{}

	StorageComponentAccess(Id id, Id nodeId, int type)
		: StorageComponentAccessData(nodeId, type)
		, id(nodeId)
	{}

	Id id;
};

#endif // STORAGE_COMPONENT_ACCESS_H
