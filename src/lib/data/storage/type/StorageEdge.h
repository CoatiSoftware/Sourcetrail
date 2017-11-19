#ifndef STORAGE_EDGE_H
#define STORAGE_EDGE_H

#include "utility/types.h"

struct StorageEdgeData
{
	StorageEdgeData()
		: type(0)
		, sourceNodeId(0)
		, targetNodeId(0)
	{}

	StorageEdgeData(int type, Id sourceNodeId, Id targetNodeId)
		: type(type)
		, sourceNodeId(sourceNodeId)
		, targetNodeId(targetNodeId)
	{}

	int type;
	Id sourceNodeId;
	Id targetNodeId;
};

struct StorageEdge: public StorageEdgeData
{
	StorageEdge()
		: StorageEdgeData()
		, id(0)
	{}

	StorageEdge(Id id, const StorageEdgeData& data)
		: StorageEdgeData(data)
		, id(id)
	{}

	StorageEdge(Id id, int type, Id sourceNodeId, Id targetNodeId)
		: StorageEdgeData(type, sourceNodeId, targetNodeId)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_EDGE_H
