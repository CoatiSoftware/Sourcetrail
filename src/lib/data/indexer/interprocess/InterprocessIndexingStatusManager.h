#ifndef INTERPROCESS_INDEXING_STATUS_MANAGER_H
#define INTERPROCESS_INDEXING_STATUS_MANAGER_H

#include <set>

#include "utility/file/FilePath.h"
#include "BaseInterprocessDataManager.h"

class InterprocessIndexingStatusManager
	: public BaseInterprocessDataManager
{
public:
	InterprocessIndexingStatusManager(const std::string& instanceUuid, Id processId, bool isOwner);
	virtual ~InterprocessIndexingStatusManager();

	void startIndexingSourceFile(const FilePath& filePath);
	void finishIndexingSourceFile();

	Id getNextFinishedProcessId();

	std::vector<FilePath> getCurrentlyIndexedSourceFilePaths();
	std::vector<FilePath> getCrashedSourceFilePaths();

	std::set<FilePath> getIndexedFiles();
	void addIndexedFiles(std::set<FilePath> filePaths);

private:
	static const char* s_sharedMemoryNamePrefix;

	static const char* s_indexingFilesKeyName;
	static const char* s_currentFilesKeyName;
	static const char* s_crashedFilesKeyName;
	static const char* s_indexedFilesKeyName;
	static const char* s_finishedProcessIdsKeyName;
};

#endif // INTERPROCESS_INDEXING_STATUS_MANAGER_H
