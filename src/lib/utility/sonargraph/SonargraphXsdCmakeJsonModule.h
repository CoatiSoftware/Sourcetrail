#ifndef SONARGRAPH_XSD_CMAKE_JSON_MODULE_H
#define SONARGRAPH_XSD_CMAKE_JSON_MODULE_H

#include "utility/sonargraph/SonargraphXsdAbstractModule.h"
#include "utility/sonargraph/SonargraphXsdRootPathWithFiles.h"

namespace Sonargraph
{
	class XsdCmakeJsonModule : public XsdAbstractModule
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdCmakeJsonModule> create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		LanguageType getSupportedLanguage() const override;

		std::set<FilePath> getAllSourcePaths() const override;
		std::set<FilePath> getAllSourceFilePathsCanonical() const override;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const override;
		std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const override;

	protected:
		XsdCmakeJsonModule() = default;
		bool init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		std::vector<XsdRootPathWithFiles::SourceFile> getIncludedSourceFilesForRootPath(
			std::shared_ptr<XsdRootPath> rootPath,
			const FilePath& baseDir,
			const std::set<FilePathFilter>& excludeFilters,
			const std::set<FilePathFilter>& includeFilters) const;

		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommandsForRootPath(
			std::shared_ptr<XsdRootPathWithFiles> rootPath, 
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const;

		std::vector<std::shared_ptr<XsdRootPathWithFiles>> m_rootPathWithFiles;
	};
}

#endif // SONARGRAPH_XSD_CMAKE_JSON_MODULE_H
