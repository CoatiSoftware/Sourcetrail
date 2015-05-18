#include "data/parser/ParseLocation.h"

ParseLocation::ParseLocation()
	: filePath("")
	, startLineNumber(0)
	, startColumnNumber(0)
	, endLineNumber(0)
	, endColumnNumber(0)
{
}

ParseLocation::ParseLocation(
	const std::string& filePath,
	uint lineNumber,
	uint columnNumber
)
	: filePath(filePath)
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
}

ParseLocation::ParseLocation(
	const std::string& filePath,
	uint startLineNumber, uint startColumnNumber,
	uint endLineNumber, uint endColumnNumber
)
	: filePath(filePath)
	, startLineNumber(startLineNumber)
	, startColumnNumber(startColumnNumber)
	, endLineNumber(endLineNumber)
	, endColumnNumber(endColumnNumber)
{
}

bool ParseLocation::isValid() const
{
	return filePath.size() > 0;
}
