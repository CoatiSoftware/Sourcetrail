#include "ParseLocation.h"

ParseLocation::ParseLocation()
	: fileId(0), startLineNumber(0), startColumnNumber(0), endLineNumber(0), endColumnNumber(0)
{
}

ParseLocation::ParseLocation(Id fileId, size_t lineNumber, size_t columnNumber)
	: fileId(fileId)
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
}

ParseLocation::ParseLocation(
	Id fileId,
	size_t startLineNumber,
	size_t startColumnNumber,
	size_t endLineNumber,
	size_t endColumnNumber)
	: fileId(fileId)
	, startLineNumber(startLineNumber)
	, startColumnNumber(startColumnNumber)
	, endLineNumber(endLineNumber)
	, endColumnNumber(endColumnNumber)
{
}

bool ParseLocation::isValid() const
{
	return fileId;
}
