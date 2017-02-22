#include "CxxProject.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "settings/ApplicationSettings.h"

#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/indexer/IndexerCxxCdb.h"
#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

#include "Application.h"

CxxProject::CxxProject(
	std::shared_ptr<CxxProjectSettings> projectSettings, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
	: Project(storageAccessProxy, dialogView)
	, m_projectSettings(projectSettings)
{
}

CxxProject::~CxxProject()
{
}

std::shared_ptr<ProjectSettings> CxxProject::getProjectSettings()
{
	return m_projectSettings;
}

const std::shared_ptr<ProjectSettings> CxxProject::getProjectSettings() const
{
	return m_projectSettings;
}

bool CxxProject::prepareRefresh()
{
	FilePath cdbPath = m_projectSettings->getAbsoluteCompilationDatabasePath();
	if (!cdbPath.empty() && !cdbPath.exists())
	{
		MessageStatus("Can't refresh project").dispatch();

		if (Application::getInstance()->hasGUI())
		{
			std::vector<std::string> options;
			options.push_back("Ok");
			Application::getInstance()->handleDialog(
				"Can't refresh. The compilation database of the project does not exist anymore: " + cdbPath.str(),
				options
			);
		}
		return false;
	}
	return true;
}

std::vector<std::shared_ptr<IndexerCommand>> CxxProject::getIndexerCommands()
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::string languageStandard = m_projectSettings->getStandard();

	std::vector<FilePath> systemHeaderSearchPaths;
	utility::append(systemHeaderSearchPaths, m_projectSettings->getAbsoluteHeaderSearchPaths());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath : getSourcePaths())
	{
		if (sourcePath.isDirectory())
		{
			systemHeaderSearchPaths.push_back(sourcePath);
		}
	}

	// Add all subdirectories of the header search paths
	if (m_projectSettings->getUseSourcePathsForHeaderSearch())
	{
		std::vector<FilePath> headerSearchSubPaths;
		for (const FilePath& sourcePath : getSourcePaths())
		{
			utility::append(headerSearchSubPaths, FileSystem::getSubDirectories(sourcePath));
		}

		utility::append(systemHeaderSearchPaths, utility::unique(headerSearchSubPaths));
	}

	std::vector<FilePath> frameworkSearchPaths;
	utility::append(frameworkSearchPaths, m_projectSettings->getAbsoluteFrameworkSearchPaths());
	utility::append(frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	std::vector<std::string> compilerFlags = m_projectSettings->getCompilerFlags();

	std::set<FilePath> indexedPaths;
	for (FilePath p: m_projectSettings->getAbsoluteSourcePaths())
	{
		if (p.exists())
		{
			indexedPaths.insert(p);
		}
	}

	std::set<FilePath> excludedPaths;
	for (FilePath p: m_projectSettings->getAbsoluteExcludePaths())
	{
		if (p.exists())
		{
			excludedPaths.insert(p);
		}
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	FilePath cdbPath = m_projectSettings->getAbsoluteCompilationDatabasePath();
	if (cdbPath.exists())
	{
		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
			(clang::tooling::JSONCompilationDatabase::loadFromFile(cdbPath.str(), error));
		for (clang::tooling::CompileCommand command: cdb->getAllCompileCommands())
		{
			std::vector<std::string> currentCompilerFlags = compilerFlags;
			currentCompilerFlags.insert(currentCompilerFlags.end(), command.CommandLine.begin(), command.CommandLine.end());

			indexerCommands.push_back(std::make_shared<IndexerCommandCxxCdb>(
				FilePath(command.Filename),
				indexedPaths,
				excludedPaths,
				FilePath(command.Directory),
				currentCompilerFlags,
				systemHeaderSearchPaths,
				frameworkSearchPaths
			));
		}
	}
	else
	{
		for (const FilePath& sourcePath: getSourceFilePaths())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxxManual>(
				sourcePath,
				indexedPaths,
				excludedPaths,
				languageStandard,
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				compilerFlags
			));
		}
	}

	return indexerCommands;
}

void CxxProject::updateFileManager(FileManager& fileManager)
{
	std::vector<FilePath> sourcePaths = m_projectSettings->getAbsoluteSourcePaths();
	std::vector<FilePath> headerPaths = sourcePaths;

	std::vector<std::string> sourceExtensions;

	FilePath cdbPath = m_projectSettings->getAbsoluteCompilationDatabasePath();
	if (cdbPath.exists())
	{
		sourcePaths = IndexerCxxCdb::getSourceFilesFromCDB(cdbPath);
	}
	else
	{
		sourceExtensions = m_projectSettings->getSourceExtensions();
	}

	std::vector<FilePath> excludePaths = m_projectSettings->getAbsoluteExcludePaths();

	fileManager.setPaths(sourcePaths, headerPaths, excludePaths, sourceExtensions);
}
