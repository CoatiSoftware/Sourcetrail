#ifndef STORAGE_SOURCE_LOCATION_H
#define STORAGE_SOURCE_LOCATION_H

#include "utility/types.h"

struct StorageSourceLocationData
{
	StorageSourceLocationData()
		: fileNodeId(0)
		, startLine(-1)
		, startCol(-1)
		, endLine(-1)
		, endCol(-1)
		, type(0)
	{}

	StorageSourceLocationData(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
		: fileNodeId(fileNodeId)
		, startLine(startLine)
		, startCol(startCol)
		, endLine(endLine)
		, endCol(endCol)
		, type(type)
	{}

	bool operator<(const StorageSourceLocationData& other) const
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
		else if (endCol != other.endCol)
		{
			return endCol < other.endCol;
		}
		else
		{
			return type < other.type;
		}
	}

	Id fileNodeId;
	uint startLine;
	uint startCol;
	uint endLine;
	uint endCol;
	int type;
};

struct StorageSourceLocation: public StorageSourceLocationData
{
	StorageSourceLocation()
		: StorageSourceLocationData()
		, id(0)
	{}

	StorageSourceLocation(Id id, const StorageSourceLocationData& data)
		: StorageSourceLocationData(data)
		, id(id)
	{}

	StorageSourceLocation(Id id, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
		: StorageSourceLocationData(fileNodeId, startLine, startCol, endLine, endCol, type)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_SOURCE_LOCATION_H
