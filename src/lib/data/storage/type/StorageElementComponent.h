#ifndef STORAGE_ELEMENT_COMPONENT_H
#define STORAGE_ELEMENT_COMPONENT_H

#include <string>

#include "types.h"

struct StorageElementComponentData
{
	StorageElementComponentData()
		: elementId(0)
		, type(0)
		, data(L"")
	{}

	StorageElementComponentData(Id elementId, int type, std::wstring data)
		: elementId(elementId)
		, type(type)
		, data(std::move(data))
	{}

	Id elementId;
	int type;
	std::wstring data;
};

struct StorageElementComponent : public StorageElementComponentData
{
	StorageElementComponent()
		: StorageElementComponentData()
		, id(0)
	{}

	StorageElementComponent(Id id, const StorageElementComponentData& data)
		: StorageElementComponentData(data)
		, id(id)
	{}

	StorageElementComponent(Id id, Id elementId, int type, std::wstring data)
		: StorageElementComponentData(elementId, type, data)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_ELEMENT_COMPONENT_H
