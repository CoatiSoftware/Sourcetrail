#ifndef PARSE_TYPE_USAGE_H
#define PARSE_TYPE_USAGE_H

#include <memory>
#include "data/type/DataType.h"
#include "data/parser/ParseLocation.h"

struct ParseTypeUsage
{
	ParseTypeUsage(const ParseLocation& location, const std::shared_ptr<DataType> dataType);

	const ParseLocation location;
	const std::shared_ptr<DataType> dataType;
};

#endif // PARSE_TYPE_USAGE_H
