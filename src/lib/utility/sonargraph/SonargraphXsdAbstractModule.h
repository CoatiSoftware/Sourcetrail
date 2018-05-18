#ifndef SONARGRAPH_XSD_ABSTRACT_MODULE_H
#define SONARGRAPH_XSD_ABSTRACT_MODULE_H

#include <set>
#include <string>
#include <vector>

#include "utility/file/FilePathFilter.h"

class ApplicationSettings;
class IndexerCommand;
class SourceGroupSettings;
class TiXmlElement;
enum LanguageType;

namespace Sonargraph
{
	class XsdRootPath;
	class SoftwareSystem;

	class XsdAbstractModule
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdAbstractModule> create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		virtual ~XsdAbstractModule() = default;

		virtual LanguageType getSupportedLanguage() const = 0;

		std::wstring getName() const;
		std::wstring getDescription() const;
		std::vector<FilePathFilter> getExcludeFilters() const;
		std::vector<FilePathFilter> getIncludeFilters() const;
		std::vector<std::shared_ptr<XsdRootPath>> getRootPaths() const;

		virtual std::set<FilePath> getAllSourcePaths() const = 0;
		virtual std::set<FilePath> getAllSourceFilePathsCanonical() const = 0;
		virtual std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const = 0;
		virtual std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const = 0;

		std::shared_ptr<const SoftwareSystem> getSoftwareSystem() const;
		std::vector<FilePathFilter> getDerivedExcludeFilters() const;
		std::vector<FilePathFilter> getDerivedIncludeFilters() const;
		virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const = 0;

	protected:
		XsdAbstractModule() = default;
		bool init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		std::weak_ptr<SoftwareSystem> m_parent;

		std::wstring m_name;
		std::wstring m_description;
		std::vector<FilePathFilter> m_excludeFilters;
		std::vector<FilePathFilter> m_includeFilters;
		std::vector<std::shared_ptr<XsdRootPath>> m_rootPaths;
	};
}

#endif // SONARGRAPH_XSD_ABSTRACT_MODULE_H
