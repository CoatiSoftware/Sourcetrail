#ifndef SONARGRAPH_SOFTWARE_SYSTEM_H
#define SONARGRAPH_SOFTWARE_SYSTEM_H

#include <string>
#include <vector>

#include "utility/file/FilePathFilter.h"
#include "utility/sonargraph/SonargraphXsdAbstractModule.h"
#include "utility/sonargraph/SonargraphXsdAbstractSystemExtension.h"

class ApplicationSettings;
class IndexerCommand;
class SourceGroupSettings;
class TiXmlElement;
enum LanguageType;

namespace Sonargraph
{
	class SoftwareSystem : public std::enable_shared_from_this<SoftwareSystem>
	{
	public:
		static std::shared_ptr<SoftwareSystem> create(const TiXmlElement* element, const FilePath& baseDirectory, LanguageType targetLanguage);

		std::wstring getName() const;
		std::string getVersion() const;
		std::wstring getDescription() const;
		std::vector<FilePathFilter> getExcludeFilters() const;
		std::vector<FilePathFilter> getIncludeFilters() const;
		const std::vector<std::shared_ptr<XsdAbstractModule>> getModules() const;
		const std::vector<std::shared_ptr<XsdAbstractSystemExtension>> getSystemExtensions() const;

		FilePath getBaseDirectory() const;
		std::set<FilePath> getAllSourcePaths() const;
		std::set<FilePath> getAllSourceFilePathsCanonical() const;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const;
		std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const;

		template <typename ExtensionType>
		std::vector<std::shared_ptr<ExtensionType>> getSpecificSystemExtensions() const;

		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const;

	protected:
		SoftwareSystem() = default;
		bool init(const TiXmlElement* element, const FilePath& baseDirectory, LanguageType targetLanguage);

		FilePath m_baseDirectory;

		std::wstring m_name;
		std::string m_version;
		std::wstring m_description;

		std::vector<FilePathFilter> m_excludeFilters;
		std::vector<FilePathFilter> m_includeFilters;
		std::vector<std::shared_ptr<XsdAbstractModule>> m_modules;
		std::vector<std::shared_ptr<XsdAbstractSystemExtension>> m_systemExtensions;
	};

	template <typename ExtensionType>
	std::vector<std::shared_ptr<ExtensionType>> SoftwareSystem::getSpecificSystemExtensions() const
	{
		std::vector<std::shared_ptr<ExtensionType>> systemExtensions;
		for (std::shared_ptr<XsdAbstractSystemExtension> systemExtension : getSystemExtensions())
		{
			if (std::shared_ptr<ExtensionType> castSystemExtension = std::dynamic_pointer_cast<ExtensionType>(systemExtension))
			{
				systemExtensions.push_back(castSystemExtension);
			}
		}
		return systemExtensions;
	}
}

#endif // SONARGRAPH_SOFTWARE_SYSTEM_H
