#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <string>

#include "utility/types.h"

#include "data/DefinitionType.h"

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
		, definitionType(definitionTypeToInt(DEFINITION_NONE))
	{}

	StorageNode(Id id, int type, const std::string& serializedName, int definitionType)
		: id(id)
		, type(type)
		, serializedName(serializedName)
		, definitionType(definitionType)
	{}

	Id id;
	int type;
	std::string serializedName;
	int definitionType;
};

struct StorageFile
{
	StorageFile()
		: id(0)
		, name("")
		, filePath("")
		, modificationTime("")
	{}

	StorageFile(Id id, const std::string& name, const std::string& filePath, const std::string& modificationTime)
		: id(id)
		, name(name)
		, filePath(filePath)
		, modificationTime(modificationTime)
	{}

	Id id;
	std::string name;
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
		, elementId(0)
		, fileNodeId(0)
		, startLine(-1)
		, startCol(-1)
		, endLine(-1)
		, endCol(-1)
		, type(0)
	{}

	StorageSourceLocation(Id id, Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
		: id(id)
		, elementId(elementId)
		, fileNodeId(fileNodeId)
		, startLine(startLine)
		, startCol(startCol)
		, endLine(endLine)
		, endCol(endCol)
		, type(type)
	{}

	Id id;
	Id elementId;
	Id fileNodeId;
	uint startLine;
	uint startCol;
	uint endLine;
	uint endCol;
	int type;
};

struct StorageComponentAccess
{
	StorageComponentAccess()
		: memberEdgeId(0)
		, type(0)
	{}

	StorageComponentAccess(Id memberEdgeId, int type)
		: memberEdgeId(memberEdgeId)
		, type(type)
	{}

	Id memberEdgeId;
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
		: message("")
		, fatal(0)
		, filePath("")
		, lineNumber(-1)
		, columnNumber(-1)
	{}

	StorageError(const std::string& message, bool fatal, const std::string& filePath, uint lineNumber, uint columnNumber)
		: message(message)
		, fatal(fatal)
		, filePath(filePath)
		, lineNumber(lineNumber)
		, columnNumber(columnNumber)
	{}

	std::string message;
	bool fatal;
	std::string filePath;
	uint lineNumber;
	uint columnNumber;
};

#endif // STORAGE_TYPES_H
