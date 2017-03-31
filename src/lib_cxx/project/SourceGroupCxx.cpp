#include "project/SourceGroupCxx.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/indexer/IndexerCxxCdb.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

#include "Application.h"

SourceGroupCxx::SourceGroupCxx(std::shared_ptr<SourceGroupSettingsCxx> settings)
	: m_settings(settings)
{
}

SourceGroupCxx::~SourceGroupCxx()
{
}

SourceGroupType SourceGroupCxx::getType() const
{
	return m_settings->getType();
}

bool SourceGroupCxx::prepareRefresh()
{
	FilePath cdbPath = m_settings->getAbsoluteCompilationDatabasePath();
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

void SourceGroupCxx::fetchAllSourceFilePaths()
{
	std::vector<FilePath> sourcePaths;

	FilePath cdbPath = m_settings->getAbsoluteCompilationDatabasePath();
	if (cdbPath.exists())
	{
		sourcePaths = IndexerCxxCdb::getSourceFilesFromCDB(cdbPath);
	}
	else
	{
		sourcePaths = m_settings->getAbsoluteSourcePaths();
	}

	m_sourceFilePathsToIndex.clear();
	FileManager fileManager;
	fileManager.update(sourcePaths, m_settings->getAbsoluteExcludePaths(), m_settings->getSourceExtensions());
	m_allSourceFilePaths = fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxx::getIndexerCommands(const bool fullRefresh)
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;
	utility::append(systemHeaderSearchPaths, m_settings->getAbsoluteHeaderSearchPaths());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath: m_settings->getAbsoluteSourcePaths())
	{
		if (sourcePath.isDirectory())
		{
			systemHeaderSearchPaths.push_back(sourcePath);
		}
	}

	// Add all subdirectories of the header search paths
	if (m_settings->getUseSourcePathsForHeaderSearch())
	{
		std::vector<FilePath> headerSearchSubPaths;
		for (const FilePath& sourcePath : m_settings->getAbsoluteSourcePaths())
		{
			utility::append(headerSearchSubPaths, FileSystem::getSubDirectories(sourcePath));
		}

		utility::append(systemHeaderSearchPaths, utility::unique(headerSearchSubPaths));
	}

	std::vector<FilePath> frameworkSearchPaths;
	utility::append(frameworkSearchPaths, m_settings->getAbsoluteFrameworkSearchPaths());
	utility::append(frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	const std::vector<std::string> compilerFlags = m_settings->getCompilerFlags();

	std::set<FilePath> indexedPaths;
	for (FilePath p: m_settings->getAbsoluteSourcePaths())
	{
		if (p.exists())
		{
			indexedPaths.insert(p);
		}
	}

	std::set<FilePath> excludedPaths;
	for (FilePath p: m_settings->getAbsoluteExcludePaths())
	{
		if (p.exists())
		{
			excludedPaths.insert(p);
		}
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());

	FilePath cdbPath = m_settings->getAbsoluteCompilationDatabasePath();
	if (cdbPath.exists())
	{
		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
			(clang::tooling::JSONCompilationDatabase::loadFromFile(cdbPath.str(), error));
		for (clang::tooling::CompileCommand command: cdb->getAllCompileCommands())
		{
			if (sourceFilePathsToIndex.find(FilePath(command.Filename)) != sourceFilePathsToIndex.end())
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
	}
	else
	{
		for (const FilePath& sourcePath: sourceFilePathsToIndex)
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxxManual>(
				sourcePath,
				indexedPaths,
				excludedPaths,
				m_settings->getStandard(),
				systemHeaderSearchPaths,
				frameworkSearchPaths,
				compilerFlags
			));
		}
	}

	return indexerCommands;
}
