#ifndef CANONICAL_FILE_PATH_CACHE_H
#define CANONICAL_FILE_PATH_CACHE_H

#include <string>
#include <unordered_map>

#include <clang/Basic/SourceManager.h>

#include "FilePath.h"
#include "FileRegister.h"

class CanonicalFilePathCache
{
public:
	CanonicalFilePathCache(std::shared_ptr<FileRegister> fileRegister);

	std::shared_ptr<FileRegister> getFileRegister() const;

	FilePath getCanonicalFilePath(const clang::FileID& fileId, const clang::SourceManager& sourceManager);
	FilePath getCanonicalFilePath(const clang::FileEntry* entry);
	FilePath getCanonicalFilePath(const std::wstring& path);

	bool isProjectFile(const clang::FileID fileId, const clang::SourceManager& sourceManager);

private:
	struct FileIdHash
	{
		size_t operator()(clang::FileID fileID) const
		{
			return fileID.getHashValue();
		}
	};

	std::shared_ptr<FileRegister> m_fileRegister;

	std::unordered_map<clang::FileID, FilePath, FileIdHash> m_fileIdMap;
	std::unordered_map<std::wstring, FilePath> m_fileStringMap;

	std::unordered_map<clang::FileID, bool, FileIdHash> m_isProjectFileMap;
};

#endif // CANONICAL_FILE_PATH_CACHE_H

