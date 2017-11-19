#ifndef STORAGE_COMMENT_LOCATION_H
#define STORAGE_COMMENT_LOCATION_H

#include "utility/types.h"

struct StorageCommentLocationData
{
	StorageCommentLocationData()
		: fileNodeId(0)
		, startLine(-1)
		, startCol(-1)
		, endLine(-1)
		, endCol(-1)
	{}

	StorageCommentLocationData(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
		: fileNodeId(fileNodeId)
		, startLine(startLine)
		, startCol(startCol)
		, endLine(endLine)
		, endCol(endCol)
	{}

	Id fileNodeId;
	uint startLine;
	uint startCol;
	uint endLine;
	uint endCol;
};

struct StorageCommentLocation: public StorageCommentLocationData
{
	StorageCommentLocation()
		: StorageCommentLocationData()
		, id(0)
	{}

	StorageCommentLocation(Id id, const StorageCommentLocationData& data)
		: StorageCommentLocationData(data)
		, id(id)
	{}

	StorageCommentLocation(Id id, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
		: StorageCommentLocationData(fileNodeId, startLine, startCol, endLine, endCol)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_COMMENT_LOCATION_H
