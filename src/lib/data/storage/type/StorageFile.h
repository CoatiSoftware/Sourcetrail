#ifndef STORAGE_FILE_H
#define STORAGE_FILE_H

#include <string>

#include "utility/types.h"

struct StorageFile
{
	StorageFile()
		: id(0)
		, filePath(L"")
		, modificationTime("")
		, indexed(true)
		, complete(true)
	{}

	StorageFile(Id id, std::wstring filePath, bool indexed, bool complete)
		: id(id)
		, filePath(std::move(filePath))
		, modificationTime("")
		, indexed(indexed)
		, complete(complete)
	{}

	StorageFile(Id id, std::wstring filePath, std::string modificationTime, bool indexed, bool complete)
		: id(id)
		, filePath(std::move(filePath))
		, modificationTime(std::move(modificationTime))
		, indexed(indexed)
		, complete(complete)
	{}

	Id id;
	std::wstring filePath;
	std::string modificationTime;
	bool indexed;
	bool complete;
};

#endif // STORAGE_FILE_H
