#ifndef PARSE_TYPE_USAGE_H
#define PARSE_TYPE_USAGE_H

#include "data/type/DataType.h"
#include "data/parser/ParseLocation.h"

struct ParseTypeUsage
{
	ParseTypeUsage(const ParseLocation& location, const DataType& type);

	const ParseLocation location;
	const DataType type;
};

#endif // PARSE_TYPE_USAGE_H
