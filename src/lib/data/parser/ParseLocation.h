#ifndef PARSE_LOCATION_H
#define PARSE_LOCATION_H

#include <string>

#include "FilePath.h"
#include "types.h"

struct ParseLocation
{
	ParseLocation();
	ParseLocation(
		FilePath filePath,
		uint lineNumber,
		uint columnNumber
	);
	ParseLocation(
		FilePath filePath,
		uint startLineNumber, uint startColumnNumber,
		uint endLineNumber, uint endColumnNumber
	);

	bool isValid() const;

	FilePath filePath;
	uint startLineNumber;
	uint startColumnNumber;
	uint endLineNumber;
	uint endColumnNumber;
};

#endif // PARSE_LOCATION_H
