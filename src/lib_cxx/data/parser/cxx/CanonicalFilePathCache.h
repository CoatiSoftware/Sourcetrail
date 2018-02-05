#ifndef CANONICAL_FILE_PATH_CACHE_H
#define CANONICAL_FILE_PATH_CACHE_H

#include <string>
#include <unordered_map>

#include "clang/Basic/FileManager.h"
#include "utility/file/FilePath.h"

class CanonicalFilePathCache
{
public:
	FilePath getCanonicalFilePath(const clang::FileEntry* entry);
	FilePath getCanonicalFilePath(const std::wstring& path);

private:
	std::unordered_map<std::wstring, FilePath> m_map;
};

#endif // CANONICAL_FILE_PATH_CACHE_H

