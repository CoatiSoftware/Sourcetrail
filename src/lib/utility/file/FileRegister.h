#ifndef FILE_REGISTER_H
#define FILE_REGISTER_H

#include <set>

#include "utility/file/FileRegisterStateData.h"
#include "utility/Cache.h"

class FileRegister
{
public:
	FileRegister(
		const FileRegisterStateData& stateData,
		const FilePath& currentPath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& excludedPaths
	);
	virtual ~FileRegister();

	const FileRegisterStateData& getStateData() const;

	void markFileIndexing(const FilePath& filePath);
	void markIndexingFilesIndexed();
	virtual bool fileIsIndexed(const FilePath& filePath) const;
	virtual bool hasFilePath(const FilePath& filePath) const;

private:
	FileRegisterStateData m_stateData;
	const FilePath& m_currentPath;
	const std::set<FilePath> m_indexedPaths;
	const std::set<FilePath> m_excludedPaths;
	mutable Cache<std::string, bool> m_hasFilePathCache;
};

#endif // FILE_REGISTER_H
