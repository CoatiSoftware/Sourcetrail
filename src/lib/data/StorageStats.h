#ifndef STORAGE_STATS_H
#define STORAGE_STATS_H

#include "data/ErrorCountInfo.h"

struct StorageStats
{
	StorageStats()
		: nodeCount(0)
		, edgeCount(0)
		, fileCount(0)
		, fileLOCCount(0)
		, errorCount(ErrorCountInfo())
	{}

	size_t nodeCount;
	size_t edgeCount;

	size_t fileCount;
	size_t fileLOCCount;

	ErrorCountInfo errorCount;
};

#endif // STORAGE_STATS_H
