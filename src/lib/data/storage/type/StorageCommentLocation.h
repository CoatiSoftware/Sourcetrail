#ifndef STORAGE_COMMENT_LOCATION_H
#define STORAGE_COMMENT_LOCATION_H

#include "types.h"

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

	bool operator<(const StorageCommentLocationData& other) const
	{
		if (fileNodeId != other.fileNodeId)
		{
			return fileNodeId < other.fileNodeId;
		}
		else if (startLine != other.startLine)
		{
			return startLine < other.startLine;
		}
		else if (startCol != other.startCol)
		{
			return startCol < other.startCol;
		}
		else if (endLine != other.endLine)
		{
			return endLine < other.endLine;
		}
		else
		{
			return endCol < other.endCol;
		}
	}

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
