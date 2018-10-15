#include "ParseLocation.h"

ParseLocation::ParseLocation()
	: fileId(0)
	, startLineNumber(0)
	, startColumnNumber(0)
	, endLineNumber(0)
	, endColumnNumber(0)
{
}

ParseLocation::ParseLocation(
	Id fileId,
	uint lineNumber,
	uint columnNumber
)
	: fileId(fileId)
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
}

ParseLocation::ParseLocation(
	Id fileId,
	uint startLineNumber, uint startColumnNumber,
	uint endLineNumber, uint endColumnNumber
)
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
