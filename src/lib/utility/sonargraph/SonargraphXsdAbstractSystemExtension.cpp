#include "SonargraphXsdAbstractSystemExtension.h"

#include "tinyxml.h"

#include "logging.h"
#include "SonargraphXsdCppSystemSettings.h"

namespace Sonargraph
{
	std::string XsdAbstractSystemExtension::getXsdTypeName()
	{
		return "xsdAbstractSystemExtension";
	}

	std::shared_ptr<XsdAbstractSystemExtension> XsdAbstractSystemExtension::create(const TiXmlElement* element)
	{
		if (std::shared_ptr<XsdCppSystemSettings> systemExtension = XsdCppSystemSettings::create(element))
		{
			return systemExtension;
		}

		return std::shared_ptr<XsdAbstractSystemExtension>();
	}

	bool XsdAbstractSystemExtension::init(const TiXmlElement* element)
	{
		if (element != nullptr)
		{
			const char* value = element->Attribute("language");
			if (value != nullptr)
			{
				m_language = value;
			}
			else
			{
				LOG_WARNING("Unable to parse \"language\" attribute of Sonargraph " + getXsdTypeName() + ".");
			}
			return true;
		}
		return false;
	}
}
