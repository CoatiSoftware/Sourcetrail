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
		if (!error && !info.fatal && info.indexed)
			return false;
		if (!fatal && info.fatal && info.indexed)
			return false;
		if (!unindexedError && !info.fatal && !info.indexed)
			return false;
		if (!unindexedFatal && info.fatal && !info.indexed)
			return false;
		return true;
	}

	bool error;
	bool fatal;

	bool unindexedError;
	bool unindexedFatal;
};

#endif // ERROR_FILTER_H
