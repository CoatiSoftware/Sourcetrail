#ifndef UTILITY_XML_H
#define UTILITY_XML_H

#include <memory>
#include <string>
#include <vector>

#include "TextAccess.h"

class TiXmlElement;

namespace utility
{
bool xmlElementHasAttribute(const TiXmlElement* element, const std::string& attributeName);

std::vector<const TiXmlElement*> getXmlChildElementsWithName(
	const TiXmlElement* parentElement, const std::string& elementName);
std::vector<const TiXmlElement*> getXmlChildElementsWithAttribute(
	const TiXmlElement* parentElement,
	const std::string& attributeName,
	const std::string& attributeValue);

std::vector<std::string> getValuesOfAllXmlElementsOnPath(
	std::shared_ptr<TextAccess> textAccess, const std::vector<std::string>& tags);
std::vector<std::string> getValuesOfAllXmlTagsByName(
	std::shared_ptr<TextAccess> textAccess, const std::string& tag);
std::vector<TiXmlElement*> getAllXmlTagsByName(TiXmlElement* root, const std::string& tag);
}	 // namespace utility

#endif	  // UTILITY_XML_H
