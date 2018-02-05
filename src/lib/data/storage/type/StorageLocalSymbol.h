#ifndef STORAGE_LOCAL_SYMBOL_H
#define STORAGE_LOCAL_SYMBOL_H

#include <string>

#include "utility/types.h"

struct StorageLocalSymbolData
{
	StorageLocalSymbolData()
		: name(L"")
	{}

	StorageLocalSymbolData(const std::wstring& name)
		: name(name)
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

	StorageLocalSymbol(Id id, const std::wstring& name)
		: StorageLocalSymbolData(name)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_LOCAL_SYMBOL_H
