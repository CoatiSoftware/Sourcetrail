#ifndef STORAGE_NODE_H
#define STORAGE_NODE_H

#include <string>

#include "utility/types.h"

struct StorageNodeData
{
	StorageNodeData()
		: type(0)
		, serializedName("")
	{}

	StorageNodeData(int type, const std::string& serializedName)
		: type(type)
		, serializedName(serializedName)
	{}

	int type;
	std::string serializedName;
};

struct StorageNode: public StorageNodeData
{
	StorageNode()
		: StorageNodeData()
		, id(0)
	{}

	StorageNode(Id id, int type, const std::string& serializedName)
		: StorageNodeData(type, serializedName)
		, id(id)
	{}

	StorageNode(Id id, const StorageNodeData& data)
		: StorageNodeData(data)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_NODE_H
