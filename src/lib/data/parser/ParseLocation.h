#ifndef PARSE_LOCATION_H
#define PARSE_LOCATION_H

#include <string>

#include "utility/types.h"

struct ParseLocation
{
	ParseLocation();
	ParseLocation(
		const std::string& filePath,
		uint startLineNumber, uint startColumnNumber,
		uint endLineNumber, uint endColumnNumber
	);

	bool isValid() const;

	std::string filePath;
	uint startLineNumber;
	uint startColumnNumber;
	uint endLineNumber;
	uint endColumnNumber;
};

#endif // PARSE_LOCATION_H
