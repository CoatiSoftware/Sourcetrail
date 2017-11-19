#ifndef STORAGE_LOCAL_SYMBOL_H
#define STORAGE_LOCAL_SYMBOL_H

#include <string>

#include "utility/types.h"

struct StorageLocalSymbolData
{
	StorageLocalSymbolData()
		: name("")
	{}

	StorageLocalSymbolData(const std::string& name)
		: name(name)
	{}

	std::string name;
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

	StorageLocalSymbol(Id id, const std::string& name)
		: StorageLocalSymbolData(name)
		, id(id)
	{}

	Id id;
};

#endif // STORAGE_LOCAL_SYMBOL_H
