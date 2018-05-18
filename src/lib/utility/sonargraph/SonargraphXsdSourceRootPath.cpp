#include "utility/sonargraph/SonargraphXsdSourceRootPath.h"

#include "tinyxml/tinyxml.h"

#include "utility/sonargraph/utilitySonargraph.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::string XsdSourceRootPath::getXsdTypeName()
	{
		return "xsdSourceRootPath";
	}

	std::shared_ptr<XsdSourceRootPath> XsdSourceRootPath::create(const TiXmlElement* element)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdSourceRootPath> rootPath = std::shared_ptr<XsdSourceRootPath>(new XsdSourceRootPath());
			if (rootPath->init(element))
			{
				return rootPath;
			}
		}
		return std::shared_ptr<XsdSourceRootPath>();
	}

	bool XsdSourceRootPath::init(const TiXmlElement* element)
	{
		return XsdRootPath::init(element);
	}
}
