#ifndef UTILITY_SONARGRAPH_H
#define UTILITY_SONARGRAPH_H

#include <string>

class TiXmlElement;

namespace utility
{
	bool sonargraphXmlElementIsType(const TiXmlElement *element, const std::string& typeName);
}

#endif // UTILITY_SONARGRAPH_H
