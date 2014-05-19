#include "data/parser/ParseLocation.h"

ParseLocation::ParseLocation(const std::string& file, unsigned int line, unsigned int column)
	: file(file)
	, line(line)
	, column(column)
{
}
