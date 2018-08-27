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

	bool operator<(const StorageEdgeData& other) const
	{
		if (type != other.type)
		{
			return type < other.type;
		}
		else if (sourceNodeId != other.sourceNodeId)
		{
			return sourceNodeId < other.sourceNodeId;
		}
		else
		{
			return targetNodeId < other.targetNodeId;
		}
	}

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
