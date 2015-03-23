#include "data/parser/ParseTypeUsage.h"

ParseTypeUsage::ParseTypeUsage(const ParseLocation& location, const std::shared_ptr<DataType> dataType)
	: location(location)
	, dataType(dataType)
{
}
