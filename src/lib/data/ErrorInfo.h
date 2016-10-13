#ifndef ERROR_INFO_H
#define ERROR_INFO_H

#include "utility/file/FilePath.h"
#include "utility/types.h"

struct ErrorInfo
{
	ErrorInfo()
		: id(0)
		, isFatal(false)
		, isIndexed(false)
	{
	}

	ErrorInfo(const std::string& message, const FilePath& filePath, Id id, bool isFatal, bool isIndexed)
		: message(message)
		, filePath(filePath)
		, id(id)
		, isFatal(isFatal)
		, isIndexed(isIndexed)
	{
	}

	std::string message;
	FilePath filePath;
	Id id;
	bool isFatal;
	bool isIndexed;
};

#endif // ERROR_INFO_H
