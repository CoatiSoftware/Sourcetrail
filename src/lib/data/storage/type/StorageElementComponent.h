#ifndef STORAGE_ELEMENT_COMPONENT_H
#define STORAGE_ELEMENT_COMPONENT_H

#include <string>

#include "types.h"

struct StorageElementComponent
{
	StorageElementComponent()
		: elementId(0)
		, type(0)
		, data(L"")
	{}

	StorageElementComponent(Id elementId, int type, std::wstring data)
		: elementId(elementId)
		, type(type)
		, data(std::move(data))
	{}

	bool operator<(const StorageElementComponent& other) const
	{
		if (elementId != other.elementId)
		{
			return elementId < other.elementId;
		}
		else if (type != other.type)
		{
			return type < other.type;
		}
		else
		{
			return data < other.data;
		}
	}

	Id elementId;
	int type;
	std::wstring data;
};

#endif // STORAGE_ELEMENT_COMPONENT_H
