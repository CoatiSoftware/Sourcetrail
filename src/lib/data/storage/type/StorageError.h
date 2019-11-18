#ifndef STORAGE_ERROR_H
#define STORAGE_ERROR_H

#include <string>

#include "FilePath.h"
#include "types.h"

struct StorageErrorData
{
	StorageErrorData(): message(L""), translationUnit(L""), fatal(0), indexed(0) {}

	StorageErrorData(std::wstring message, std::wstring translationUnit, bool fatal, bool indexed)
		: message(std::move(message))
		, translationUnit(std::move(translationUnit))
		, fatal(fatal)
		, indexed(indexed)
	{
	}

	bool operator<(const StorageErrorData& other) const
	{
		if (message != other.message)
		{
			return message < other.message;
		}
		else if (translationUnit != other.translationUnit)
		{
			return translationUnit < other.translationUnit;
		}
		else if (fatal != other.fatal)
		{
			return fatal < other.fatal;
		}
		else
		{
			return indexed < other.indexed;
		}
	}

	std::wstring message;
	std::wstring translationUnit;
	bool fatal;
	bool indexed;
};

struct StorageError: public StorageErrorData
{
	StorageError(): StorageErrorData(), id(0) {}

	StorageError(Id id, const StorageErrorData& data): StorageErrorData(data), id(id) {}

	StorageError(Id id, std::wstring message, std::wstring translationUnit, bool fatal, bool indexed)
		: StorageErrorData(std::move(message), std::move(translationUnit), fatal, indexed), id(id)
	{
	}

	Id id;
};

#endif	  // STORAGE_ERROR_H
