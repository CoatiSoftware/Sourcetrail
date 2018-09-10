#include "utility/sonargraph/SonargraphXsdCppManualModule.h"

#include "tinyxml/tinyxml.h"

#include "data/indexer/IndexerCommandCxx.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "utility/file/FileSystem.h"
#include "utility/sonargraph/SonargraphSoftwareSystem.h"
#include "utility/sonargraph/utilitySonargraph.h"
#include "utility/utility.h"
#include "utility/utilityFile.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::string XsdCppManualModule::getXsdTypeName()
	{
		return "xsdCppManualModule";
	}

	std::vector<std::wstring> XsdCppManualModule::getIncludeOptionPrefixes()
	{
		// make sure that none of these prefixes is the prefix of a prefix that appears further down in the list
		return {
			L"-I",
			L"--include_directory=",
			L"--sys_include="
		};
	}

	std::shared_ptr<XsdCppManualModule> XsdCppManualModule::create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdCppManualModule> module = std::shared_ptr<XsdCppManualModule>(new XsdCppManualModule());
			if (module->init(element, parent))
			{
				return module;
			}
		}
		return std::shared_ptr<XsdCppManualModule>();
	}

	LanguageType XsdCppManualModule::getSupportedLanguage() const
	{
		return LANGUAGE_CPP;
	}

	std::set<FilePath> XsdCppManualModule::getAllSourcePaths() const
	{
		FilePath baseDirectory;
		if (std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem())
		{
			baseDirectory = softwareSystem->getBaseDirectory();
		}

		std::set<FilePath> sourcePaths;
		for (std::shared_ptr<XsdRootPath> rootPath : getRootPaths())
		{
			sourcePaths.insert(rootPath->getFilePath(baseDirectory));
		}

		return sourcePaths;
	}

	std::set<FilePath> XsdCppManualModule::getAllSourceFilePathsCanonical() const
	{
		const std::vector<FilePathFilter> excludeFilters = getDerivedExcludeFilters();
		const std::vector<FilePathFilter> includeFilters = getDerivedIncludeFilters();

		std::set<FilePath> sourceFilePaths;

		for (const FileInfo& fileInfo : FileSystem::getFileInfosFromPaths(utility::getTopLevelPaths(getAllSourcePaths()), m_sourceFileExtensions))
		{
			const FilePath sourceFilePath = fileInfo.path.getCanonical();

			bool excludeMatches = false;
			for (const FilePathFilter& excludeFilter : excludeFilters)
			{
				if (excludeFilter.isMatching(sourceFilePath))
				{
					excludeMatches = true;
					break;
				}
			}
			if (excludeMatches)
			{
				for (const FilePathFilter& includeFilter : includeFilters)
				{
					if (includeFilter.isMatching(sourceFilePath))
					{
						excludeMatches = false;
						break;
					}
				}
			}
			if (!excludeMatches)
			{
				sourceFilePaths.insert(sourceFilePath);
			}
		}

		return sourceFilePaths;
	}

	std::set<FilePath> XsdCppManualModule::getAllCxxHeaderSearchPathsCanonical() const
	{
		std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem();
		if (!softwareSystem)
		{
			return std::set<FilePath>();
		}

		const std::vector<std::wstring> optionPrefixes = getIncludeOptionPrefixes();
		const FilePath baseDir = softwareSystem->getBaseDirectory();
		const FilePath baseIncludeDir = m_basePathForIncludes.isAbsolute() ? m_basePathForIncludes : baseDir.getConcatenated(m_basePathForIncludes);

		std::set<FilePath> headerSearchPaths;
		for (const std::wstring& compilerOption : m_moduleCompilerOptions)
		{
			for (const std::wstring& optionPrefix : optionPrefixes)
			{
				if (utility::isPrefix(optionPrefix, compilerOption))
				{
					FilePath headerSearchPath(utility::trim(compilerOption.substr(optionPrefix.size())));
					if (headerSearchPath.isAbsolute())
					{
						headerSearchPaths.insert(headerSearchPath);
					}
					else
					{
						headerSearchPaths.insert(baseIncludeDir.getConcatenated(headerSearchPath));
					}
					break;
				}
			}
		}

		return headerSearchPaths;
	}

	std::vector<std::shared_ptr<IndexerCommand>> XsdCppManualModule::getIndexerCommands(
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> appSettings) const
	{
		std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem();
		if (!softwareSystem)
		{
			return std::vector<std::shared_ptr<IndexerCommand>>();
		}

		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

		std::set<FilePath> indexedHeaderPaths;
		std::wstring languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();

		if (std::shared_ptr<const SourceGroupSettingsCxxSonargraph> sonargraphSettings =
			std::dynamic_pointer_cast<const SourceGroupSettingsCxxSonargraph>(sourceGroupSettings))
		{
			indexedHeaderPaths = utility::toSet(sonargraphSettings->getIndexedHeaderPathsExpandedAndAbsolute());
			languageStandard = sonargraphSettings->getCppStandard();
		}
		else
		{
			LOG_ERROR(L"Source group doesn't specify language standard. Falling back to \"" + languageStandard + L"\".");
			LOG_ERROR(L"Source group doesn't specify any indexed header paths");
		}

		const std::vector<FilePath> systemHeaderSearchPaths = (appSettings ? appSettings->getHeaderSearchPathsExpanded() : std::vector<FilePath>());
		const std::vector<FilePath> frameworkSearchPaths = (appSettings ? appSettings->getFrameworkSearchPathsExpanded() : std::vector<FilePath>());

		const std::set<FilePathFilter> excludeFilters = utility::toSet(getDerivedExcludeFilters());
		const std::set<FilePathFilter> includeFilters = utility::toSet(getDerivedIncludeFilters());

		std::vector<std::wstring> processedOptions;
		{
			const std::vector<std::wstring> optionPrefixes = getIncludeOptionPrefixes();
			const FilePath baseIncludeDir = m_basePathForIncludes.isAbsolute() ? m_basePathForIncludes : softwareSystem->getBaseDirectory().getConcatenated(m_basePathForIncludes);

			for (const std::wstring& compilerOption : m_moduleCompilerOptions)
			{
				bool isIncludeOption = false;
				for (const std::wstring& optionPrefix : optionPrefixes)
				{
					if (utility::isPrefix(optionPrefix, compilerOption))
					{
						isIncludeOption = true;

						processedOptions.push_back(L"-isystem");

						FilePath headerSearchPath(utility::trim(compilerOption.substr(optionPrefix.size())));
						if (headerSearchPath.isAbsolute())
						{
							processedOptions.push_back(headerSearchPath.wstr());
						}
						else
						{
							processedOptions.push_back(baseIncludeDir.getConcatenated(headerSearchPath).wstr());
						}
						break;
					}
				}

				if (!isIncludeOption)
				{
					processedOptions.push_back(compilerOption);
				}
			}
		}

		processedOptions.push_back(L"-std=" + languageStandard);

		for (const FilePath& sourceFilePath : getAllSourceFilePathsCanonical())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxx>(
				sourceFilePath,
				utility::concat(indexedHeaderPaths, { sourceFilePath }),
				excludeFilters,
				includeFilters,
				softwareSystem->getBaseDirectory(),
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				utility::concat(processedOptions, { sourceFilePath.wstr() })
			));
		}

		return indexerCommands;
	}

	XsdCppManualModule::XsdCppManualModule()
		: m_basePathForIncludes(L".")
		, m_sourceFileExtensions({L".cpp"})
	{
	}

	bool XsdCppManualModule::init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		if (!XsdAbstractModule::init(element, parent))
		{
			return false;
		}

		if (element != nullptr)
		{

			{
				const TiXmlElement* basePathForIncludesElement = element->FirstChildElement("basePathForIncludes");
				if (basePathForIncludesElement != nullptr)
				{
					const char* text = basePathForIncludesElement->GetText();
					if (text != nullptr)
					{
						m_basePathForIncludes = FilePath(utility::decodeFromUtf8(text));
					}
					else
					{
						LOG_ERROR("Unable to parse \"basePathForIncludes\" element of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}
			}
			{
				const TiXmlElement* sourceFileExtensionsElement = element->FirstChildElement("sourceFileExtensions");
				if (sourceFileExtensionsElement != nullptr)
				{
					const char* text = sourceFileExtensionsElement->GetText();
					if (text != nullptr)
					{
						m_sourceFileExtensions.clear();
						for (const std::wstring extension : utility::split<std::vector<std::wstring>>(utility::decodeFromUtf8(text), L","))
						{
							m_sourceFileExtensions.push_back(utility::trim(extension));
						}
					}
					else
					{
						LOG_ERROR("Unable to parse \"sourceFileExtensions\" element of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}
			}

			for (const TiXmlElement* moduleCompilerOptionsElement : utility::getXmlChildElementsWithName(element, "moduleCompilerOptions"))
			{
				const char* text = moduleCompilerOptionsElement->GetText();
				if (text != nullptr)
				{
					for (const std::wstring& s : utility::splitToVector(utility::decodeFromUtf8(text), L' '))
					{
						m_moduleCompilerOptions.push_back(utility::trim(s));
					}
				}
			}
			return true;
		}
		return false;
	}
}
