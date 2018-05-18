#include "utility/utilityXml.h"

#include "tinyxml/tinyxml.h"

#include "utility/logging/logging.h"

namespace utility
{
	bool xmlElementHasAttribute(const TiXmlElement* element, const std::string& attributeName)
	{
		return (element->Attribute(attributeName.c_str()) != nullptr);
	}

	std::vector<const TiXmlElement*> getXmlChildElementsWithName(const TiXmlElement* parentElement, const std::string& elementName)
	{
		std::vector<const TiXmlElement*> elements;

		const TiXmlElement* child = parentElement->FirstChildElement(elementName.c_str());

		for (child; child; child = child->NextSiblingElement(elementName.c_str()))
		{
			elements.push_back(child);
		}

		return elements;
	}

	std::vector<const TiXmlElement*> getXmlChildElementsWithAttribute(const TiXmlElement* parentElement, const std::string& attributeName, const std::string& attributeValue)
	{
		std::vector<const TiXmlElement*> elements;

		const TiXmlElement* child = parentElement->FirstChildElement();
	
		for (child; child; child = child->NextSiblingElement())
		{
			const char* value = child->Attribute(attributeName.c_str());
			if (value != nullptr && value == attributeValue)
			{
				elements.push_back(child);
			}
		}

		return elements;
	}

	std::vector<std::string> getValuesOfAllXmlElementsOnPath(std::shared_ptr<TextAccess> textAccess, const std::vector<std::string>& tags)
	{
		std::vector<std::string> values;

		std::string text = textAccess->getText();

		TiXmlDocument doc;
		const char* pTest = doc.Parse(text.c_str(), 0, TIXML_ENCODING_LEGACY);
		if (pTest != nullptr)
		{
			TiXmlHandle docHandle(&doc);
			std::vector<std::pair<TiXmlElement*, size_t>> traversalStates;
			traversalStates.push_back(std::make_pair(docHandle.ToNode()->FirstChildElement(), 0));

			while (!traversalStates.empty())
			{
				TiXmlElement* currentElement = traversalStates.back().first;
				const size_t currentIndex = traversalStates.back().second;
				traversalStates.pop_back();

				if(currentElement != nullptr &&
					currentElement->Value() == tags[currentIndex]
				)
				{
					if (currentIndex < tags.size() - 1)
					{
						TiXmlElement* nextElement = currentElement->FirstChildElement();

						while (nextElement)
						{
							traversalStates.push_back(std::make_pair(nextElement, size_t(currentIndex + 1)));
							nextElement = nextElement->NextSiblingElement();
						}
					}
					else
					{
						if (currentElement->FirstChild() &&
							currentElement->FirstChild() == currentElement->LastChild() &&
							currentElement->FirstChild()->ToText()
							)
						{
							values.push_back(currentElement->FirstChild()->ToText()->Value());
						}
					}
				}
			}
		}
		else
		{
			if (doc.Error())
			{
				LOG_ERROR(std::string("Error while parsing XML: ") + doc.ErrorDesc() + " (in line " + std::to_string(doc.ErrorRow()) + ": \"" + textAccess->getLine(doc.ErrorRow()) + "\")");

			}
			else
			{
				LOG_ERROR("Unable to load file.");
			}
		}
		return values;
	}

	std::vector<std::string> getValuesOfAllXmlTagsByName(std::shared_ptr<TextAccess> textAccess, const std::string& tag)
	{
		std::vector<std::string> values;

		std::string text = textAccess->getText();

		TiXmlDocument doc;
		const char* pTest = doc.Parse(text.c_str(), 0, TIXML_ENCODING_UTF8);
		if (pTest != nullptr)
		{
			TiXmlHandle docHandle(&doc);
			TiXmlElement *rootElement = docHandle.ToNode()->FirstChildElement();
			if(rootElement != nullptr)
			{
				for (TiXmlElement* element: getAllXmlTagsByName(rootElement, tag))
				{
					if (element->FirstChild() &&
						element->FirstChild() == element->LastChild() &&
						element->FirstChild()->ToText()
					)
					{
						values.push_back(element->FirstChild()->ToText()->Value());
					}
				}
			}
			else
			{
				//	LOG_ERROR("Unable to load file.");
			}
		}
		else
		{
		//	LOG_ERROR("Unable to load file.");
		}
		return values;
	}

	std::vector<TiXmlElement*> getAllXmlTagsByName(TiXmlElement* root, const std::string& tag)
	{
		std::vector<TiXmlElement*> nodes;

		TiXmlElement* element = root;

		while (element)
		{
			std::string value = element->Value();

			if (value == tag)
			{
				nodes.push_back(element);
			}

			if (element->FirstChildElement() != nullptr)
			{
				element = element->FirstChildElement();
			}
			else if (element->NextSiblingElement() != nullptr)
			{
				element = element->NextSiblingElement();
			}
			else
			{
				if (element == nullptr)
				{
				}

				while (element->Parent()->ToElement() != nullptr && element->Parent()->NextSiblingElement() == nullptr)
				{
					TiXmlElement* newElement = element->Parent()->ToElement();

					if (newElement == nullptr)
					{
					}

					element = newElement;
				}
				if (element->Parent() != nullptr && element->Parent()->NextSiblingElement() != nullptr)
				{
					element = element->Parent()->NextSiblingElement();
				}
				else
				{
					break;
				}
			}
		}

		return nodes;
	}
}
