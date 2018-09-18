#ifndef SONARGRAPH_PROJECT_H
#define SONARGRAPH_PROJECT_H

#include <set>
#include <memory>
#include <vector>

#include "LanguageType.h"

// xsdCppModule
//	xsdCppManualModule -> basePathForIncludes(0, n), sourceFileExtensions(0, n), moduleCompilerOptions(0, n)
//	xsdCmakeJsonModule -> rootPathWithFiles(0, n)
//	xsdCppMakefileModule -> makefile(1), additionalCompilerOptions(0, 1)
//	xsdCppVsProjectFileModule -> projectFile(1,1)
//	xsdCppCaptureModule -> captureFile(1,1)

class ApplicationSettings;
class FilePath;
class IndexerCommand;
class SourceGroupSettings;
class TextAccess;

namespace Sonargraph
{
	class SoftwareSystem;

	class Project
	{
	public:
		static std::shared_ptr<Project> load(const FilePath& projectFilePath, LanguageType targetLanguage);
		static std::shared_ptr<Project> load(std::shared_ptr<TextAccess> xmlAccess, LanguageType targetLanguage);

		int getLoadedModuleCount() const;

		std::set<FilePath> getAllSourceFilePathsCanonical() const;
		std::set<FilePath> getAllCxxHeaderSearchPathsCanonical() const;

		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
			std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
			std::shared_ptr<const ApplicationSettings> appSettings) const;

	private:
		Project() = default;

		std::shared_ptr<SoftwareSystem> m_softwareSystem;
	};
}

#endif // SONARGRAPH_PROJECT_H
