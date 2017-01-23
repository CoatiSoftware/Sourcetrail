#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <string>

#include "utility/file/FilePath.h"
#include "utility/types.h"

#include "data/DefinitionKind.h"

struct StorageEdge
{
	StorageEdge()
		: id(0)
		, type(0)
		, sourceNodeId(0)
		, targetNodeId(0)
	{}

	StorageEdge(Id id, int type, Id sourceNodeId, Id targetNodeId)
		: id(id)
		, type(type)
		, sourceNodeId(sourceNodeId)
		, targetNodeId(targetNodeId)
	{}

	Id id;
	int type;
	Id sourceNodeId;
	Id targetNodeId;
};

struct StorageNode
{
	StorageNode()
		: id(0)
		, type(0)
		, serializedName("")
	{}

	StorageNode(Id id, int type, const std::string& serializedName)
		: id(id)
		, type(type)
		, serializedName(serializedName)
	{}

	Id id;
	int type;
	std::string serializedName;
};

struct StorageSymbol
{
	StorageSymbol()
		: id(0)
		, definitionKind(definitionKindToInt(DEFINITION_NONE))
	{}

	StorageSymbol(Id id, int definitionKind)
		: id(id)
		, definitionKind(definitionKind)
	{}

	Id id;
	int definitionKind;
};

struct StorageFile
{
	StorageFile()
		: id(0)
		, filePath("")
		, modificationTime("")
	{}

	StorageFile(Id id, const std::string& filePath, const std::string& modificationTime)
		: id(id)
		, filePath(filePath)
		, modificationTime(modificationTime)
	{}

	Id id;
	std::string filePath;
	std::string modificationTime;
};

struct StorageLocalSymbol
{
	StorageLocalSymbol()
		: id(0)
		, name("")
	{}

	StorageLocalSymbol(Id id, const std::string& name)
		: id(id)
		, name(name)
	{}

	Id id;
	std::string name;
};

struct StorageSourceLocation
{
	StorageSourceLocation()
		: id(0)
		, fileNodeId(0)
		, startLine(-1)
		, startCol(-1)
		, endLine(-1)
		, endCol(-1)
		, type(0)
	{}

	StorageSourceLocation(Id id, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
		: id(id)
		, fileNodeId(fileNodeId)
		, startLine(startLine)
		, startCol(startCol)
		, endLine(endLine)
		, endCol(endCol)
		, type(type)
	{}

	Id id;
	Id fileNodeId;
	uint startLine;
	uint startCol;
	uint endLine;
	uint endCol;
	int type;
};

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

	Id elementId;
	Id sourceLocationId;
};

struct StorageComponentAccess
{
	StorageComponentAccess()
		: nodeId(0)
		, type(0)
	{}

	StorageComponentAccess(Id nodeId, int type)
		: nodeId(nodeId)
		, type(type)
	{}

	Id nodeId;
	int type;
};

struct StorageCommentLocation
{
	StorageCommentLocation()
		: id(0)
		, fileNodeId(0)
		, startLine(-1)
		, startCol(-1)
		, endLine(-1)
		, endCol(-1)
	{}

	StorageCommentLocation(Id id, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
		: id(id)
		, fileNodeId(fileNodeId)
		, startLine(startLine)
		, startCol(startCol)
		, endLine(endLine)
		, endCol(endCol)
	{}

	Id id;
	Id fileNodeId;
	uint startLine;
	uint startCol;
	uint endLine;
	uint endCol;
};

struct StorageError
{
	StorageError()
		: id(0)
		, message("")
		, lineNumber(-1)
		, columnNumber(-1)
		, fatal(0)
		, indexed(0)
	{}

	StorageError(
		Id id,
		const std::string& message,
		const FilePath& filePath,
		uint lineNumber,
		uint columnNumber,
		bool fatal,
		bool indexed
	)
		: id(id)
		, message(message)
		, filePath(filePath)
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
		, fatal(fatal)
		, indexed(indexed)
	{}

	Id id;
	std::string message;

	FilePath filePath;
	uint lineNumber;
	uint columnNumber;

	bool fatal;
	bool indexed;
};

#endif // STORAGE_TYPES_H
