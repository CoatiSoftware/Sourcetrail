#include "utility/sonargraph/SonargraphSourceRootPath.h"

#include "tinyxml/tinyxml.h"

#include "data/indexer/IndexerCommandJava.h"
#include "utility/logging/logging.h"
#include "utility/sonargraph/SonargraphXsdAbstractModule.h"
#include "utility/sonargraph/SonargraphXsdRootPathWithFiles.h"
#include "utility/sonargraph/utilitySonargraph.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::string SourceRootPath::getXsdTypeName()
	{
		return "sourceRootPath";
	}

	std::shared_ptr<SourceRootPath> SourceRootPath::create(const TiXmlElement* element)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<SourceRootPath> rootPath = std::shared_ptr<SourceRootPath>(new SourceRootPath());
			if (rootPath->init(element))
			{
				return rootPath;
			}
		}

		return std::shared_ptr<SourceRootPath>();
	}

	std::wstring SourceRootPath::getName() const
	{
		return m_name;
	}

	FilePath SourceRootPath::getFilePath(const FilePath& baseDirectory) const
	{
		FilePath filePath(getName());
		if (filePath.isAbsolute())
		{
			return filePath;
		}
		return baseDirectory.getConcatenated(filePath);
	}

	bool SourceRootPath::init(const TiXmlElement* element)
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
