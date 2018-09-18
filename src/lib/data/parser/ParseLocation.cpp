#include "ParseLocation.h"

ParseLocation::ParseLocation()
	: filePath(L"")
	, startLineNumber(0)
	, startColumnNumber(0)
	, endLineNumber(0)
	, endColumnNumber(0)
{
}

ParseLocation::ParseLocation(
	FilePath filePath,
	uint lineNumber,
	uint columnNumber
)
	: filePath(std::move(filePath.makeCanonical()))
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
}

ParseLocation::ParseLocation(
	FilePath filePath,
	uint startLineNumber, uint startColumnNumber,
	uint endLineNumber, uint endColumnNumber
)
	: filePath(std::move(filePath.makeCanonical()))
	, startLineNumber(startLineNumber)
	, startColumnNumber(startColumnNumber)
	, endLineNumber(endLineNumber)
	, endColumnNumber(endColumnNumber)
{
}

bool ParseLocation::isValid() const
{
	return !filePath.empty();
}
