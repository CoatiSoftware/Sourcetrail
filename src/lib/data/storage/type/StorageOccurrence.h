#ifndef STORAGE_OCCURRENCE_H
#define STORAGE_OCCURRENCE_H

#include "types.h"

struct StorageOccurrence
{
	StorageOccurrence()
		: elementId(0)
		, sourceLocationId(0)
	{}

	StorageOccurrence(Id elementId, Id sourceLocationId)
		: elementId(elementId)
		, sourceLocationId(sourceLocationId)
	{}

	bool operator<(const StorageOccurrence& other) const
	{
		if (elementId != other.elementId)
		{
			return elementId < other.elementId;
		}
		else
		{
			return sourceLocationId < other.sourceLocationId;
		}
	}

	Id elementId;
	Id sourceLocationId;
};

#endif // STORAGE_OCCURRENCE_H
