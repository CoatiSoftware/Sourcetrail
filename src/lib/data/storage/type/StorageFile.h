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
		, complete(true)
	{}

	StorageFile(Id id, const std::wstring& filePath, const std::string& modificationTime, bool complete)
		: id(id)
		, filePath(filePath)
		, modificationTime(modificationTime)
		, complete(complete)
	{}

	Id id;
	std::wstring filePath;
	std::string modificationTime;
	bool complete;
};

#endif // STORAGE_FILE_H
