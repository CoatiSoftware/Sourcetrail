#ifndef PARSE_LOCATION_H
#define PARSE_LOCATION_H

#include <string>

#include "FilePath.h"
#include "types.h"

enum class ParseLocationType
{
	TOKEN,
	SCOPE,
	SIGNATURE,
	QUALIFIER,
	LOCAL
};

struct ParseLocation
{
	ParseLocation();
	ParseLocation(
		Id fileId,
		uint lineNumber,
		uint columnNumber
	);
	ParseLocation(
		Id fileId,
		uint startLineNumber, uint startColumnNumber,
		uint endLineNumber, uint endColumnNumber
	);

	bool isValid() const;

	Id fileId;
	uint startLineNumber;
	uint startColumnNumber;
	uint endLineNumber;
	uint endColumnNumber;
};

#endif // PARSE_LOCATION_H
