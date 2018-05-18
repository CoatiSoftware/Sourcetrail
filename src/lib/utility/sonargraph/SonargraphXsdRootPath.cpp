#include "utility/sonargraph/SonargraphXsdRootPath.h"

#include "tinyxml/tinyxml.h"

#include "data/indexer/IndexerCommandJava.h"
#include "utility/logging/logging.h"
#include "utility/sonargraph/SonargraphXsdAbstractModule.h"
#include "utility/sonargraph/SonargraphXsdRootPathWithFiles.h"
#include "utility/sonargraph/SonargraphXsdSourceRootPath.h"
#include "utility/sonargraph/utilitySonargraph.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::string XsdRootPath::getXsdTypeName()
	{
		return "xsdRootPath";
	}

	std::shared_ptr<XsdRootPath> XsdRootPath::create(const TiXmlElement* element)
	{
		if (std::shared_ptr<XsdRootPathWithFiles> rootPath = XsdRootPathWithFiles::create(element))
		{
			return rootPath;
		}
		if (std::shared_ptr<XsdSourceRootPath> rootPath = XsdSourceRootPath::create(element))
		{
			return rootPath;
		}

		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdRootPath> rootPath = std::shared_ptr<XsdRootPath>(new XsdRootPath());
			if (rootPath->init(element))
			{
				return rootPath;
			}
		}

		return std::shared_ptr<XsdRootPath>();
	}

	std::wstring XsdRootPath::getName() const
	{
		return m_name;
	}

	FilePath XsdRootPath::getFilePath(const FilePath& baseDirectory) const
	{
		FilePath filePath(getName());
		if (filePath.isAbsolute())
		{
			return filePath;
		}
		return baseDirectory.getConcatenated(filePath);
	}

	bool XsdRootPath::init(const TiXmlElement* element)
	{
		if (element != nullptr)
		{
			const char* value = element->Attribute("name");
			if (value != nullptr)
			{
				m_name = utility::decodeFromUtf8(value);
			}
			else
			{
				LOG_ERROR("Unable to parse \"name\" of Sonargraph " + getXsdTypeName() + ".");
				return false;
			}
			return true;
		}
		return false;
	}
}
