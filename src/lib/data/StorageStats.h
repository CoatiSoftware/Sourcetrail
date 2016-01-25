#ifndef STORAGE_STATS_H
#define STORAGE_STATS_H

struct StorageStats
{
	StorageStats()
		: nodeCount(0)
		, edgeCount(0)
		, charCount(0)
		, wordCount(0)
		, searchNodeCount(0)
		, fileCount(0)
		, fileLOCCount(0)
		, sourceLocationCount(0)
		, errorCount(0)
	{}

	size_t nodeCount;
	size_t edgeCount;

	size_t charCount;
	size_t wordCount;
	size_t searchNodeCount;

	size_t fileCount;
	size_t fileLOCCount;
	size_t sourceLocationCount;

	size_t errorCount;
};

#endif // STORAGE_STATS_H
