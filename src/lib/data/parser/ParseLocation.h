#ifndef PARSE_LOCATION_H
#define PARSE_LOCATION_H

#include <string>

struct ParseLocation
{
	ParseLocation(
		const std::string& filePath,
		unsigned int startLineNumber, unsigned int startColumnNumber,
		unsigned int endLineNumber, unsigned int endColumnNumber);

	bool isValid() const;

	const std::string filePath;
	unsigned int startLineNumber;
	unsigned int startColumnNumber;
	unsigned int endLineNumber;
	unsigned int endColumnNumber;
};

#endif // PARSE_LOCATION_H
