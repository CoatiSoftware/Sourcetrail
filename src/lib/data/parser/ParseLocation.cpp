#include "data/parser/ParseLocation.h"

ParseLocation::ParseLocation()
	: filePath(L"")
	, startLineNumber(0)
	, startColumnNumber(0)
	, endLineNumber(0)
	, endColumnNumber(0)
{
}

ParseLocation::ParseLocation(
	const FilePath& filePath,
	uint lineNumber,
	uint columnNumber
)
	: filePath(filePath.getCanonical())
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
}

ParseLocation::ParseLocation(
	const FilePath& filePath,
	uint startLineNumber, uint startColumnNumber,
	uint endLineNumber, uint endColumnNumber
)
	: filePath(filePath.getCanonical())
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
