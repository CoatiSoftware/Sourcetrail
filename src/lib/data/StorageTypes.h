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
		, complete(true)
	{}

	StorageFile(Id id, const std::string& filePath, const std::string& modificationTime, bool complete)
		: id(id)
		, filePath(filePath)
		, modificationTime(modificationTime)
		, complete(complete)
	{}

	Id id;
	std::string filePath;
	std::string modificationTime;
	bool complete;
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
		: id(0)
		, nodeId(0)
		, type(0)
	{}

	StorageComponentAccess(Id id, Id nodeId, int type)
		: id(id)
		, nodeId(nodeId)
		, type(type)
	{}

	Id id;
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





struct StorageBookmarkCategory
{
	StorageBookmarkCategory()
		: id(0)
		, name("")
	{}

	StorageBookmarkCategory(
		Id id,
		const std::string& name
	)
		: id(id)
		, name(name)
	{}

	Id id;
	std::string name;
};

struct StorageBookmark
{
	StorageBookmark()
		: id(0)
		, name("")
		, comment("")
		, timestamp("")
		, categoryId(0)
	{}

	StorageBookmark(
		Id id,
		const std::string& name,
		const std::string& comment,
		const std::string& timestamp,
		const Id categoryId
	)
		: id(id)
		, name(name)
		, comment(comment)
		, timestamp(timestamp)
		, categoryId(categoryId)
	{}

	Id id;
	std::string name;
	std::string comment;
	std::string timestamp;
	Id categoryId;
};

struct StorageBookmarkedNode
{
	StorageBookmarkedNode()
		: id(0)
		, bookmarkId(0)
		, serializedNodeName("")
	{}

	StorageBookmarkedNode(
		Id id,
		Id bookmarkId,
		const std::string& serializedNodeName
	)
		: id(id)
		, bookmarkId(bookmarkId)
		, serializedNodeName(serializedNodeName)
	{}

	Id id;
	Id bookmarkId;
	std::string serializedNodeName;
};

struct StorageBookmarkedEdge
{
	StorageBookmarkedEdge()
		: id(0)
		, bookmarkId(0)
		, serializedSourceNodeName("")
		, serializedTargetNodeName("")
		, edgeType(0)
		, sourceNodeActive(false)
	{}

	StorageBookmarkedEdge(
		Id id,
		Id bookmarkId,
		const std::string& serializedSourceNodeName,
		const std::string& serializedTargetNodeName,
		int edgeType,
		bool sourceNodeActive
	)
		: id(id)
		, bookmarkId(bookmarkId)
		, serializedSourceNodeName(serializedSourceNodeName)
		, serializedTargetNodeName(serializedTargetNodeName)
		, edgeType(edgeType)
		, sourceNodeActive(sourceNodeActive)
	{}

	Id id;
	Id bookmarkId;
	std::string serializedSourceNodeName;
	std::string serializedTargetNodeName;
	int edgeType;
	bool sourceNodeActive;
};

#endif // STORAGE_TYPES_H
