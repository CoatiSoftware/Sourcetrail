#ifndef STORAGE_ERROR_H
#define STORAGE_ERROR_H

#include <string>

#include "utility/file/FilePath.h"
#include "utility/types.h"

struct StorageErrorData
{
	StorageErrorData()
		: message("")
		, lineNumber(-1)
		, columnNumber(-1)
		, fatal(0)
		, indexed(0)
	{}

	StorageErrorData(
		const std::string& message,
		const FilePath& filePath,
		uint lineNumber,
		uint columnNumber,
		bool fatal,
		bool indexed
	)
		: message(message)
		, filePath(filePath)
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
		, fatal(fatal)
		, indexed(indexed)
	{}

	std::string message;

	FilePath filePath;
	uint lineNumber;
	uint columnNumber;

	bool fatal;
	bool indexed;
};

struct StorageError: public StorageErrorData
{
	StorageError()
		: StorageErrorData()
		, id(0)
	{}

	StorageError(Id id, const StorageErrorData& data)
		: StorageErrorData(data)
		, id(id)
	{}

	StorageError(
		Id id,
		const std::string& message,
		const FilePath& filePath,
		uint lineNumber,
		uint columnNumber,
		bool fatal,
		bool indexed
	)
		: StorageErrorData(
			message,
			filePath,
			lineNumber,
			columnNumber,
			fatal,
			indexed
		)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_ERROR_H
