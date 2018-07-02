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
		const std::wstring& message,
		const std::wstring& filePath,
		uint lineNumber,
		uint columnNumber,
		const std::wstring& translationUnit,
		bool fatal,
		bool indexed
	)
		: message(message)
		, filePath(filePath)
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
		, translationUnit(translationUnit)
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
		const std::wstring& message,
		const std::wstring& filePath,
		uint lineNumber,
		uint columnNumber,
		const std::wstring& translationUnit,
		bool fatal,
		bool indexed
	)
		: StorageErrorData(
			message,
			filePath,
			lineNumber,
			columnNumber,
			translationUnit,
			fatal,
			indexed
		)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_ERROR_H
