#ifndef STORAGE_LOCAL_SYMBOL_H
#define STORAGE_LOCAL_SYMBOL_H

#include <string>

#include "utility/types.h"

struct StorageLocalSymbolData
{
	StorageLocalSymbolData()
		: name(L"")
	{}

	StorageLocalSymbolData(std::wstring name)
		: name(std::move(name))
	{}

	std::wstring name;
};

struct StorageLocalSymbol: public StorageLocalSymbolData
{
	StorageLocalSymbol()
		: StorageLocalSymbolData()
		, id(0)
	{}

	StorageLocalSymbol(Id id, const StorageLocalSymbolData& data)
		: StorageLocalSymbolData(data)
		, id(id)
	{}

	StorageLocalSymbol(Id id, std::wstring name)
		: StorageLocalSymbolData(std::move(name))
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_LOCAL_SYMBOL_H
