#include "utility/sonargraph/SonargraphSoftwareSystem.h"

#include "tinyxml/tinyxml.h"

#include "data/indexer/IndexerCommand.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"
#include "utility/utility.h"

namespace Sonargraph
{
	std::shared_ptr<SoftwareSystem> SoftwareSystem::create(const TiXmlElement* element, const FilePath& baseDirectory, LanguageType targetLanguage)
	{
		std::shared_ptr<SoftwareSystem> softwareSystem = std::shared_ptr<SoftwareSystem>(new SoftwareSystem());
		if (softwareSystem->init(element, baseDirectory, targetLanguage))
		{
			return softwareSystem;
		}
		return std::shared_ptr<SoftwareSystem>();
	}

	std::wstring SoftwareSystem::getName() const
	{
		return m_name;
	}

	std::string SoftwareSystem::getVersion() const
	{
		return m_version;
	}

	std::wstring SoftwareSystem::getDescription() const
	{
		return m_description;
	}

	std::vector<FilePathFilter> SoftwareSystem::getExcludeFilters() const
	{
		return m_excludeFilters;
	}

	std::vector<FilePathFilter> SoftwareSystem::getIncludeFilters() const
	{
		return m_includeFilters;
	}

	const std::vector<std::shared_ptr<XsdAbstractModule>> SoftwareSystem::getModules() const
	{
		return m_modules;
	}

	const std::vector<std::shared_ptr<XsdAbstractSystemExtension>> SoftwareSystem::getSystemExtensions() const
	{
		return m_systemExtensions;
	}

	FilePath SoftwareSystem::getBaseDirectory() const
	{
		return m_baseDirectory;
	}

	std::set<FilePath> SoftwareSystem::getAllSourcePaths() const
	{
		std::set<FilePath> sourcePaths;
		for (std::shared_ptr<XsdAbstractModule> module : m_modules)
		{
			utility::append(sourcePaths, module->getAllSourcePaths());
		}
		return sourcePaths;
	}

	std::set<FilePath> SoftwareSystem::getAllSourceFilePathsCanonical() const
	{
		std::set<FilePath> sourceFilePaths;
		for (std::shared_ptr<XsdAbstractModule> module : m_modules)
		{
			utility::append(sourceFilePaths, module->getAllSourceFilePathsCanonical());
		}
		return sourceFilePaths;
	}

	std::set<FilePath> SoftwareSystem::getAllCxxHeaderSearchPathsCanonical() const
	{
		std::set<FilePath> sourceFilePaths;
		for (std::shared_ptr<XsdAbstractModule> module : m_modules)
		{
			utility::append(sourceFilePaths, module->getAllCxxHeaderSearchPathsCanonical());
		}
		return sourceFilePaths;
	}

	std::set<FilePath> SoftwareSystem::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
	{
		std::set<FilePath> containedFilePaths;
		for (std::shared_ptr<XsdAbstractModule> module : m_modules)
		{
			utility::append(containedFilePaths, module->filterToContainedFilePaths(filePaths));
		}
		return containedFilePaths;
	}

	std::vector<std::shared_ptr<IndexerCommand>> SoftwareSystem::getIndexerCommands(
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> appSettings) const
	{
		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
		for (std::shared_ptr<XsdAbstractModule> module : m_modules)
		{
			utility::append(indexerCommands, module->getIndexerCommands(sourceGroupSettings, appSettings));
		}
		return indexerCommands;
	}

	bool SoftwareSystem::init(const TiXmlElement* element, const FilePath& baseDirectory, LanguageType targetLanguage)
	{
		m_baseDirectory = baseDirectory;
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
					LOG_INFO("Unable to parse \"name\" attribute of Sonargraph softwareSystem.");
				}
			}

			{
				const char* value = element->Attribute("version");
				if (value != nullptr)
				{
					m_version = value;
				}
				else
				{
					LOG_WARNING("Unable to parse \"version\" attribute of Sonargraph softwareSystem.");
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

			for (const TiXmlElement* moduleElement : utility::getXmlChildElementsWithName(element, "module"))
			{
				if (std::shared_ptr<XsdAbstractModule> module = XsdAbstractModule::create(moduleElement, shared_from_this()))
				{
					if (module->getSupportedLanguage() == targetLanguage)
					{
						m_modules.push_back(module);
					}
					else
					{
						LOG_INFO(L"Discarding Sonargraph module \"" + module->getName() + L"\" because it does not match the Sourcetrail project's language type.");
					}
				}
				else
				{
					LOG_ERROR("Unable to parse \"module\" element of Sonargraph softwareSystem.");
					return false;
				}
			}

			for (const TiXmlElement* systemExtensionElement : utility::getXmlChildElementsWithName(element, "systemExtension"))
			{
				if (std::shared_ptr<XsdAbstractSystemExtension> systemExtension = XsdAbstractSystemExtension::create(systemExtensionElement))
				{
					m_systemExtensions.push_back(systemExtension);
				}
				else
				{
					LOG_ERROR("Unable to parse \"systemExtension\" element of Sonargraph softwareSystem.");
					return false;
				}
			}
			return true;
		}
		return false;
	}
}
