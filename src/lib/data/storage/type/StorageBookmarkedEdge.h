#ifndef STORAGE_BOOKMARKED_EDGE_H
#define STORAGE_BOOKMARKED_EDGE_H

#include <string>

#include "types.h"

struct StorageBookmarkedEdgeData
{
	StorageBookmarkedEdgeData()
		: bookmarkId(0)
		, serializedSourceNodeName(L"")
		, serializedTargetNodeName(L"")
		, edgeType(0)
		, sourceNodeActive(false)
	{}

	StorageBookmarkedEdgeData(
		Id bookmarkId,
		const std::wstring& serializedSourceNodeName,
		const std::wstring& serializedTargetNodeName,
		int edgeType,
		bool sourceNodeActive
	)
		: bookmarkId(bookmarkId)
		, serializedSourceNodeName(serializedSourceNodeName)
		, serializedTargetNodeName(serializedTargetNodeName)
		, edgeType(edgeType)
		, sourceNodeActive(sourceNodeActive)
	{}

	Id bookmarkId;
	std::wstring serializedSourceNodeName;
	std::wstring serializedTargetNodeName;
	int edgeType;
	bool sourceNodeActive;
};

struct StorageBookmarkedEdge: public StorageBookmarkedEdgeData
{
	StorageBookmarkedEdge()
		: StorageBookmarkedEdgeData()
		, id(0)
	{}

	StorageBookmarkedEdge(Id id, const StorageBookmarkedEdgeData& data)
		: StorageBookmarkedEdgeData(data)
		, id(id)
	{}

	StorageBookmarkedEdge(
		Id id,
		Id bookmarkId,
		const std::wstring& serializedSourceNodeName,
		const std::wstring& serializedTargetNodeName,
		int edgeType,
		bool sourceNodeActive
	)
		: StorageBookmarkedEdgeData(
			bookmarkId, 
			serializedSourceNodeName, 
			serializedTargetNodeName, 
			edgeType, 
			sourceNodeActive
		)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_BOOKMARKED_EDGE_H
