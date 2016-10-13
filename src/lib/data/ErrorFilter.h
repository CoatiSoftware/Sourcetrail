#ifndef ERROR_FILTER_H
#define ERROR_FILTER_H

#include "data/ErrorInfo.h"
#include "data/StorageTypes.h"

struct ErrorFilter
{
	ErrorFilter()
		: error(true)
		, fatal(true)
		, unindexedError(false)
		, unindexedFatal(true)
	{
	}

	bool filter(const ErrorInfo& info) const
	{
		if (!error && !info.isFatal && info.isIndexed)
			return false;
		if (!fatal && info.isFatal && info.isIndexed)
			return false;
		if (!unindexedError && !info.isFatal && !info.isIndexed)
			return false;
		if (!unindexedFatal && info.isFatal && !info.isIndexed)
			return false;
		return true;
	}

	bool filter(const StorageError& storageError) const
	{
		if (!error && !storageError.fatal && storageError.indexed)
			return false;
		if (!fatal && storageError.fatal && storageError.indexed)
			return false;
		if (!unindexedError && !storageError.fatal && !storageError.indexed)
			return false;
		if (!unindexedFatal && storageError.fatal && !storageError.indexed)
			return false;
		return true;
	}

	bool error;
	bool fatal;

	bool unindexedError;
	bool unindexedFatal;
};

#endif // ERROR_FILTER_H
