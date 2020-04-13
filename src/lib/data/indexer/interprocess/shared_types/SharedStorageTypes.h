#ifndef SHARED_STORAGE_TYPES_H
#define SHARED_STORAGE_TYPES_H

#include "../../../../utility/interprocess/SharedMemory.h"
#include "../../../storage/type/StorageComponentAccess.h"
#include "../../../storage/type/StorageEdge.h"
#include "../../../storage/type/StorageError.h"
#include "../../../storage/type/StorageFile.h"
#include "../../../storage/type/StorageLocalSymbol.h"
#include "../../../storage/type/StorageNode.h"
#include "../../../storage/type/StorageOccurrence.h"
#include "../../../storage/type/StorageSourceLocation.h"
#include "../../../storage/type/StorageSymbol.h"
#include "../../../../utility/types.h"
#include "../../../../../lib_utility/utility/utilityString.h"

// macro creating SharedStorageType from StorageType
// - arguments: StorageType & SharedStorageType
// - defines: conversion functions toShared() & fromShared()

#define CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(__type__, __shared_type__)                             \
	typedef __type__ __shared_type__;                                                              \
                                                                                                   \
	inline const __shared_type__& toShared(                                                        \
		const __type__& instance, SharedMemory::Allocator* allocator)                              \
	{                                                                                              \
		return instance;                                                                           \
	}                                                                                              \
                                                                                                   \
	inline const __type__& fromShared(const __shared_type__& instance)                             \
	{                                                                                              \
		return instance;                                                                           \
	}

CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(StorageEdge, SharedStorageEdge)
CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(StorageSymbol, SharedStorageSymbol)
CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(StorageSourceLocation, SharedStorageSourceLocation)
CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(StorageOccurrence, SharedStorageOccurrence)
CONVERT_STORAGE_TYPE_TO_SHARED_TYPE(StorageComponentAccess, SharedStorageComponentAccess)


struct SharedStorageNode
{
	SharedStorageNode(
		Id id, int type, const std::string& serializedName, SharedMemory::Allocator* allocator)
		: id(id), type(type), serializedName(serializedName.c_str(), allocator)
	{
	}

	Id id;
	int type;
	SharedMemory::String serializedName;
};

inline SharedStorageNode toShared(const StorageNode& node, SharedMemory::Allocator* allocator)
{
	return SharedStorageNode(
		node.id, node.type, utility::encodeToUtf8(node.serializedName), allocator);
}

inline StorageNode fromShared(const SharedStorageNode& node)
{
	return StorageNode(node.id, node.type, utility::decodeFromUtf8(node.serializedName.c_str()));
}


struct SharedStorageFile
{
	SharedStorageFile(
		Id id,
		const std::string& filePath,
		const std::string& languageIdentifier,
		bool indexed,
		bool complete,
		SharedMemory::Allocator* allocator)
		: id(id)
		, filePath(filePath.c_str(), allocator)
		, languageIdentifier(languageIdentifier.c_str(), allocator)
		, indexed(indexed)
		, complete(complete)
	{
	}

	Id id;
	SharedMemory::String filePath;
	SharedMemory::String languageIdentifier;
	bool indexed;
	bool complete;
};

inline SharedStorageFile toShared(const StorageFile& file, SharedMemory::Allocator* allocator)
{
	return SharedStorageFile(
		file.id,
		utility::encodeToUtf8(file.filePath),
		utility::encodeToUtf8(file.languageIdentifier),
		file.indexed,
		file.complete,
		allocator);
}

inline StorageFile fromShared(const SharedStorageFile& file)
{
	return StorageFile(
		file.id,
		utility::decodeFromUtf8(file.filePath.c_str()),
		utility::decodeFromUtf8(file.languageIdentifier.c_str()),
		"",
		file.indexed,
		file.complete);
}


struct SharedStorageLocalSymbol
{
	SharedStorageLocalSymbol(Id id, const std::string& name, SharedMemory::Allocator* allocator)
		: id(id), name(name.c_str(), allocator)
	{
	}

	Id id;
	SharedMemory::String name;
};

inline SharedStorageLocalSymbol toShared(
	const StorageLocalSymbol& symbol, SharedMemory::Allocator* allocator)
{
	return SharedStorageLocalSymbol(symbol.id, utility::encodeToUtf8(symbol.name), allocator);
}

inline StorageLocalSymbol fromShared(const SharedStorageLocalSymbol& symbol)
{
	return StorageLocalSymbol(symbol.id, utility::decodeFromUtf8(symbol.name.c_str()));
}


struct SharedStorageError
{
	SharedStorageError(
		Id id,
		const std::string& message,
		const std::string& translationUnit,
		bool fatal,
		bool indexed,
		SharedMemory::Allocator* allocator)
		: id(id)
		, message(message.c_str(), allocator)
		, translationUnit(translationUnit.c_str(), allocator)
		, fatal(fatal)
		, indexed(indexed)
	{
	}

	Id id;
	SharedMemory::String message;
	SharedMemory::String translationUnit;
	bool fatal;
	bool indexed;
};

inline SharedStorageError toShared(const StorageError& error, SharedMemory::Allocator* allocator)
{
	return SharedStorageError(
		error.id,
		utility::encodeToUtf8(error.message),
		utility::encodeToUtf8(error.translationUnit),
		error.fatal,
		error.indexed,
		allocator);
}

inline StorageError fromShared(const SharedStorageError& error)
{
	return StorageError(
		error.id,
		utility::decodeFromUtf8(error.message.c_str()),
		utility::decodeFromUtf8(error.translationUnit.c_str()),
		error.fatal,
		error.indexed);
}

#endif	  // SHARED_STORAGE_TYPES_H
