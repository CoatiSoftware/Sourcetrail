#include "data/parser/ParseLocation.h"

ParseLocation::ParseLocation(
	const std::string& filePath,
	unsigned int startLineNumber, unsigned int startColumnNumber,
	unsigned int endLineNumber, unsigned int endColumnNumber
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
	return startLineNumber > 0 && endLineNumber >= startLineNumber;
}
