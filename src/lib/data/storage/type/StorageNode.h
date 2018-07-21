#ifndef STORAGE_NODE_H
#define STORAGE_NODE_H

#include <string>

#include "utility/types.h"

struct StorageNodeData
{
	StorageNodeData()
		: type(0)
		, serializedName(L"")
	{}

	StorageNodeData(int type, std::wstring serializedName)
		: type(type)
		, serializedName(std::move(serializedName))
	{}

	int type;
	std::wstring serializedName;
};

struct StorageNode: public StorageNodeData
{
	StorageNode()
		: StorageNodeData()
		, id(0)
	{}

	StorageNode(Id id, int type, std::wstring serializedName)
		: StorageNodeData(type, std::move(serializedName))
		, id(id)
	{}

	StorageNode(Id id, const StorageNodeData& data)
		: StorageNodeData(data)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_NODE_H
