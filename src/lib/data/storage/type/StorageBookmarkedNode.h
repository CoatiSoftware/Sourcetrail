#ifndef STORAGE_BOOKMARKED_NODE_H
#define STORAGE_BOOKMARKED_NODE_H

#include <string>

#include "utility/types.h"

struct StorageBookmarkedNodeData
{
	StorageBookmarkedNodeData()
		: bookmarkId(0)
		, serializedNodeName("")
	{}

	StorageBookmarkedNodeData(Id bookmarkId, const std::string& serializedNodeName)
		: bookmarkId(bookmarkId)
		, serializedNodeName(serializedNodeName)
	{}

	Id bookmarkId;
	std::string serializedNodeName;
};

struct StorageBookmarkedNode: public StorageBookmarkedNodeData
{
	StorageBookmarkedNode()
		: StorageBookmarkedNodeData()
		, id(0)
	{}

	StorageBookmarkedNode(Id id, const StorageBookmarkedNodeData& data)
		: StorageBookmarkedNodeData(data)
		, id(id)
	{}

	StorageBookmarkedNode(
		Id id,
		Id bookmarkId,
		const std::string& serializedNodeName
	)
		: StorageBookmarkedNodeData(bookmarkId, serializedNodeName)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_BOOKMARKED_NODE_H
