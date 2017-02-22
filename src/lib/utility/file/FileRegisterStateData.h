#ifndef FILE_REGISTER_STATE_DATA_H
#define FILE_REGISTER_STATE_DATA_H

#include <mutex>
#include <map>

#include "utility/file/FilePath.h"

class FileRegisterStateData
{
public:
	FileRegisterStateData();
	FileRegisterStateData(const FileRegisterStateData& o);

	void inject(const FileRegisterStateData& o);

	void markFileIndexing(const FilePath& filePath);
	void markIndexingFilesIndexed();
	bool fileIsIndexed(const FilePath& filePath) const;

private:
	enum IndexingState
	{
		STATE_NON_INDEXED,
		STATE_INDEXING,
		STATE_INDEXED
	};

	std::map<FilePath, IndexingState> m_filePaths;
	mutable std::mutex m_filePathsMutex;
};

#endif // FILE_REGISTER_STATE_DATA_H
