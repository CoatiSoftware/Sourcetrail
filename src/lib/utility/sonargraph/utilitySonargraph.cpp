#include "utilitySonargraph.h"

#include "tinyxml.h"

namespace utility
{
	bool sonargraphXmlElementIsType(const TiXmlElement *element, const std::string& typeName)
	{
		const char* value = element->Attribute("xsi:type");
		return (value != nullptr && value == "ns4:" + typeName);
	}
}
