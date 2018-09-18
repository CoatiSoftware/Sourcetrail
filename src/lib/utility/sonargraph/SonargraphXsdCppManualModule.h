#ifndef SONARGRAPH_XSD_CPP_MANUAL_MODULE_H
#define SONARGRAPH_XSD_CPP_MANUAL_MODULE_H

#include "SonargraphXsdAbstractModule.h"
#include "SonargraphXsdRootPathWithFiles.h"

namespace Sonargraph
{
	class XsdCppManualModule : public XsdAbstractModule
	{
	public:
		static std::string getXsdTypeName();
		static std::vector<std::wstring> getIncludeOptionPrefixes();
		static std::shared_ptr<XsdCppManualModule> create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		LanguageType getSupportedLanguage() const override;

		std::set<FilePath> getAllSourcePaths() const override;
		std::set<FilePath> getAllSourceFilePathsCanonical() const override;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const override;
		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const override;

	protected:
		XsdCppManualModule();
		bool init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		FilePath m_basePathForIncludes;
		std::vector<std::wstring> m_sourceFileExtensions;
		std::vector<std::wstring> m_moduleCompilerOptions;
	};
}

#endif // SONARGRAPH_XSD_CPP_MANUAL_MODULE_H
