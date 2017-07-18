#ifndef STORAGE_STATS_H
#define STORAGE_STATS_H

#include "utility/TimePoint.h"

struct StorageStats
{
	StorageStats()
		: nodeCount(0)
		, edgeCount(0)
		, fileCount(0)
		, completedFileCount(0)
		, fileLOCCount(0)
	{}

	size_t nodeCount;
	size_t edgeCount;

	size_t fileCount;
	size_t completedFileCount;
	size_t fileLOCCount;

	TimePoint timestamp;
};

#endif // STORAGE_STATS_H
