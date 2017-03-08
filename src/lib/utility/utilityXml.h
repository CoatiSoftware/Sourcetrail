#ifndef UTILITY_XML_H
#define UTILITY_XML_H

#include <memory>
#include <string>
#include <vector>

#include "utility/text/TextAccess.h"

class TiXmlElement;

namespace utility
{
	std::vector<std::string> getValuesOfAllXmlElementsOnPath(std::shared_ptr<TextAccess> textAccess, const std::vector<std::string>& tags);
	std::vector<std::string> getValuesOfAllXmlTagsByName(std::shared_ptr<TextAccess> textAccess, const std::string& tag);
	std::vector<TiXmlElement*> getAllXmlTagsByName(TiXmlElement* root, const std::string& tag);
}

#endif // UTILITY_XML_H
