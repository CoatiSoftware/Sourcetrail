#ifndef STORAGE_ERROR_H
#define STORAGE_ERROR_H

#include <string>

#include "utility/file/FilePath.h"
#include "utility/types.h"

struct StorageErrorData
{
	StorageErrorData()
		: message(L"")
		, filePath(L"")
		, lineNumber(-1)
		, columnNumber(-1)
		, translationUnit(L"")
		, fatal(0)
		, indexed(0)
	{}

	StorageErrorData(
		std::wstring message,
		std::wstring filePath,
		uint lineNumber,
		uint columnNumber,
		std::wstring translationUnit,
		bool fatal,
		bool indexed
	)
		: message(std::move(message))
		, filePath(std::move(filePath))
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
		, translationUnit(std::move(translationUnit))
		, fatal(fatal)
		, indexed(indexed)
	{}

	std::wstring message;

	std::wstring filePath;
	uint lineNumber;
	uint columnNumber;

	std::wstring translationUnit;
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
		std::wstring message,
		std::wstring filePath,
		uint lineNumber,
		uint columnNumber,
		std::wstring translationUnit,
		bool fatal,
		bool indexed
	)
		: StorageErrorData(
			std::move(message),
			std::move(filePath),
			lineNumber,
			columnNumber,
			std::move(translationUnit),
			fatal,
			indexed
		)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_ERROR_H
