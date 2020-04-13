#ifndef FILE_REGISTER_H
#define FILE_REGISTER_H

#include <set>

#include "FilePath.h"
#include "../UnorderedCache.h"

class FilePathFilter;

class FileRegister
{
public:
	FileRegister(
		const FilePath& currentPath,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePathFilter>& excludeFilters);
	virtual ~FileRegister();

	virtual bool hasFilePath(const FilePath& filePath) const;

private:
	const FilePath& m_currentPath;
	const std::set<FilePath> m_indexedPaths;
	const std::set<FilePathFilter> m_excludeFilters;
	mutable UnorderedCache<std::wstring, bool> m_hasFilePathCache;
};

#endif	  // FILE_REGISTER_H
