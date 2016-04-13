#ifndef ERROR_INFO_H
#define ERROR_INFO_H

#include "utility/file/FilePath.h"
#include "utility/types.h"

struct ErrorInfo
{
	ErrorInfo()
		: id(0)
		, isFatal(false)
	{
	}

	ErrorInfo(const std::string& message, const FilePath& filePath, Id id, bool isFatal)
		: message(message)
		, filePath(filePath)
		, id(id)
		, isFatal(isFatal)
	{
	}

	std::string message;
	FilePath filePath;
	Id id;
	bool isFatal;
};

#endif // ERROR_INFO_H
