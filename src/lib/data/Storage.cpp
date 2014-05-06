#include "data/Storage.h"

#include <sstream>

#include "utility/logging/logging.h"

Storage::Storage()
{
}

Storage::~Storage()
{
}

void Storage::addClass(const ParseObject& object)
{
	std::stringstream info;
	info << "class " << object.name
		<< " <" << object.fileName << " " << object.lineNumber << ":" << object.columnNumber << ">";
	LOG_INFO(info.str());
}
