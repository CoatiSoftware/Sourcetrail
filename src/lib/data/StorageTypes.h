#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <string>

#include "utility/types.h"

struct StorageEdge
{
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
	StorageLocalSymbol(Id id, const std::string& name)
		: id(id)
		, name(name)
	{}

	Id id;
	std::string name;
};

struct StorageSourceLocation
{
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
	StorageComponentAccess(Id memberEdgeId, int type)
		: memberEdgeId(memberEdgeId)
		, type(type)
	{}

	Id memberEdgeId;
	int type;
};

struct StorageCommentLocation
{
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
