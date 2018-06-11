#include "utility/sonargraph/SonargraphXsdAbstractModule.h"

#include "tinyxml/tinyxml.h"

#include "utility/logging/logging.h"
#include "utility/sonargraph/SonargraphXsdCmakeJsonModule.h"
#include "utility/sonargraph/SonargraphXsdCppManualModule.h"
#include "utility/sonargraph/SonargraphXsdJavaModule.h"
#include "utility/sonargraph/SonargraphXsdRootPath.h"
#include "utility/sonargraph/SonargraphSoftwareSystem.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"
#include "utility/utility.h"

namespace Sonargraph
{
	std::string XsdAbstractModule::getXsdTypeName()
	{
		return "xsdAbstractModule";
	}

	std::shared_ptr<XsdAbstractModule> XsdAbstractModule::create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		if (std::shared_ptr<XsdAbstractModule> module = XsdCmakeJsonModule::create(element, parent))
		{
			return module;
		}
		if (std::shared_ptr<XsdCppManualModule> module = XsdCppManualModule::create(element, parent))
		{
			return module;
		}
		if (std::shared_ptr<XsdJavaModule> module = XsdJavaModule::create(element, parent))
		{
			return module;
		}

		return std::shared_ptr<XsdAbstractModule>();
	}

	std::wstring XsdAbstractModule::getName() const
	{
		return m_name;
	}

	std::wstring XsdAbstractModule::getDescription() const
	{
		return m_description;
	}

	std::vector<FilePathFilter> XsdAbstractModule::getExcludeFilters() const
	{
		return m_excludeFilters;
	}

	std::vector<FilePathFilter> XsdAbstractModule::getIncludeFilters() const
	{
		return m_includeFilters;
	}

	std::vector<std::shared_ptr<XsdRootPath>> XsdAbstractModule::getRootPaths() const
	{
		return m_rootPaths;
	}

	std::shared_ptr<const SoftwareSystem> XsdAbstractModule::getSoftwareSystem() const
	{
		return m_parent.lock();
	}

	std::vector<FilePathFilter> XsdAbstractModule::getDerivedExcludeFilters() const
	{
		std::vector<FilePathFilter> excludeFilters = getExcludeFilters();
		if (std::shared_ptr<const SoftwareSystem> parent = getSoftwareSystem())
		{
			utility::append(excludeFilters, parent->getExcludeFilters());
		}
		return excludeFilters;
	}

	std::vector<FilePathFilter> XsdAbstractModule::getDerivedIncludeFilters() const
	{
		std::vector<FilePathFilter> includeFilters = getIncludeFilters();
		if (std::shared_ptr<const SoftwareSystem> parent = getSoftwareSystem())
		{
			utility::append(includeFilters, parent->getIncludeFilters());
		}
		return includeFilters;
	}

	bool XsdAbstractModule::init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		m_parent = parent;
		if (element != nullptr)
		{
			{
				const char* value = element->Attribute("name");
				if (value != nullptr)
				{
					m_name = utility::decodeFromUtf8(value);
				}
				else
				{
					LOG_WARNING("Unable to parse \"name\" attribute of Sonargraph " + getXsdTypeName() + ".");
				}
			}

			{
				const TiXmlElement* descriptionElement = element->FirstChildElement("description");
				if (descriptionElement != nullptr)
				{
					const char* text = descriptionElement->GetText();
					if (text != nullptr)
					{
						m_description = utility::decodeFromUtf8(text);
					}
				}
			}

			for (const TiXmlElement* excludeElement : utility::getXmlChildElementsWithName(element, "exclude"))
			{
				const char* text = excludeElement->GetText();
				if (text != nullptr)
				{
					m_excludeFilters.push_back(FilePathFilter(utility::decodeFromUtf8(text)));
				}
			}

			for (const TiXmlElement* includeElement : utility::getXmlChildElementsWithName(element, "include"))
			{
				const char* text = includeElement->GetText();
				if (text != nullptr)
				{
					m_includeFilters.push_back(FilePathFilter(utility::decodeFromUtf8(text)));
				}
			}

			for (const TiXmlElement* rootPathElement : utility::getXmlChildElementsWithName(element, "rootPath"))
			{
				if (std::shared_ptr<XsdRootPath> rootPath = XsdRootPath::create(rootPathElement))
				{
					m_rootPaths.push_back(rootPath);
				}
				else
				{
					LOG_ERROR("Unable to parse \"rootPath\" element of Sonargraph " + getXsdTypeName() + ".");
					return false;
				}
			}
			return true;
		}
		return false;
	}
}
