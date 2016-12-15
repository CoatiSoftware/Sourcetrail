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
	const FilePath& filePath,
	uint lineNumber,
	uint columnNumber
)
	: filePath(filePath)
	, startLineNumber(lineNumber)
	, startColumnNumber(columnNumber)
	, endLineNumber(lineNumber)
	, endColumnNumber(columnNumber)
{
	if (this->filePath.exists())
	{
		this->filePath = this->filePath.canonical();
	}
}

ParseLocation::ParseLocation(
	const FilePath& filePath,
	uint startLineNumber, uint startColumnNumber,
	uint endLineNumber, uint endColumnNumber
)
	: filePath(filePath)
	, startLineNumber(startLineNumber)
	, startColumnNumber(startColumnNumber)
	, endLineNumber(endLineNumber)
	, endColumnNumber(endColumnNumber)
{
	if (this->filePath.exists())
	{
		this->filePath = this->filePath.canonical();
	}
}

bool ParseLocation::isValid() const
{
	return !filePath.empty();
}
