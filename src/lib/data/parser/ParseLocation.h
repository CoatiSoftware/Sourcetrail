#ifndef PARSE_LOCATION_H
#define PARSE_LOCATION_H

#include <string>

struct ParseLocation
{
	ParseLocation(const std::string& file, unsigned int line, unsigned int column);

	const std::string file;
	const unsigned int line;
	const unsigned int column;
};

#endif // PARSE_LOCATION_H
