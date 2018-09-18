#ifndef SONARGRAPH_XSD_JAVA_MODULE_H
#define SONARGRAPH_XSD_JAVA_MODULE_H

#include "SonargraphXsdAbstractModule.h"

namespace Sonargraph
{
	class SourceRootPath;

	class XsdJavaModule : public XsdAbstractModule
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdJavaModule> create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		LanguageType getSupportedLanguage() const override;

		std::set<FilePath> getAllSourcePaths() const override;
		std::set<FilePath> getAllSourceFilePathsCanonical() const override;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const override;
		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const override;

	protected:
		XsdJavaModule() = default;
		bool init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent);

		std::vector<std::shared_ptr<SourceRootPath>> m_sourceRootPaths;
	};
}

#endif // SONARGRAPH_XSD_JAVA_MODULE_H
