#include "utility/sonargraph/SonargraphXsdCppSystemSettings.h"

#include "tinyxml/tinyxml.h"

#include "utility/sonargraph/utilitySonargraph.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::string XsdCppSystemSettings::getXsdTypeName()
	{
		return "xsdCppSystemSettings";
	}

	std::shared_ptr<XsdCppSystemSettings> XsdCppSystemSettings::create(const TiXmlElement* element)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdCppSystemSettings> systemExtension = std::shared_ptr<XsdCppSystemSettings>(new XsdCppSystemSettings());
			if (systemExtension->init(element))
			{
				return systemExtension;
			}
		}
		return std::shared_ptr<XsdCppSystemSettings>();
	}

	bool XsdCppSystemSettings::hasCompilerOptionsForId(Id id) const
	{
		return m_compilerOptionSets.find(id) != m_compilerOptionSets.end();
	}

	std::vector<std::wstring> XsdCppSystemSettings::getCompilerOptionsForId(Id id) const
	{
		std::map<Id, std::vector<std::wstring>>::const_iterator it = m_compilerOptionSets.find(id);
		if (it != m_compilerOptionSets.end())
		{
			return it->second;
		}
		return std::vector<std::wstring>();
	}

	bool XsdCppSystemSettings::init(const TiXmlElement* element)
	{
		XsdAbstractSystemExtension::init(element);

		if (element != nullptr)
		{
			for (const TiXmlElement* compilerOptionSetElement : utility::getXmlChildElementsWithName(element, "compilerOptionSets"))
			{
				Id optionSetId = 0;
				{
					const char* value = compilerOptionSetElement->Attribute("id");
					if (value != nullptr && atoi(value) >= 0)
					{
						optionSetId = atoi(value);
					}
					else
					{
						LOG_ERROR("Unable to parse \"id\" attribute of compilerOptionSets of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}

				for (const TiXmlElement* optionElement : utility::getXmlChildElementsWithName(compilerOptionSetElement, "option"))
				{
					const char* value = optionElement->GetText();
					if (value != nullptr)
					{
						m_compilerOptionSets[optionSetId].push_back(utility::decodeFromUtf8(value));
					}
					else
					{
						LOG_ERROR("Unable to parse \"option\" attribute of compilerOptionSets of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}
			}
			return true;
		}
		return false;
	}
}
