#include "project/SourceGroupCxxCdb.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"
#include "Application.h"

SourceGroupCxxCdb::SourceGroupCxxCdb(std::shared_ptr<SourceGroupSettingsCxxCdb> settings)
	: m_settings(settings)
{
}

SourceGroupCxxCdb::~SourceGroupCxxCdb()
{
}

SourceGroupType SourceGroupCxxCdb::getType() const
{
	return SOURCE_GROUP_CXX_CDB;
}

bool SourceGroupCxxCdb::prepareRefresh()
{
	FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (!cdbPath.empty() && !cdbPath.exists())
	{
		MessageStatus("Can't refresh project").dispatch();

		if (std::shared_ptr<Application> application = Application::getInstance())
		{
			if (application->hasGUI())
			{
				std::vector<std::string> options;
				options.push_back("Ok");
				application->handleDialog(
					"Can't refresh. The compilation database of the project does not exist anymore: " + cdbPath.str(),
					options
				);
			}
		}
		return false;
	}
	return true;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCdb::getIndexerCommands(
	std::set<FilePath>* filesToIndex, bool fullRefresh)
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;
	utility::append(systemHeaderSearchPaths, m_settings->getHeaderSearchPathsExpandedAndAbsolute());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	for (const FilePath& sourcePath: m_settings->getSourcePathsExpandedAndAbsolute())
	{
		if (sourcePath.isDirectory())
		{
			systemHeaderSearchPaths.push_back(sourcePath);
		}
	}

	std::vector<FilePath> frameworkSearchPaths;
	utility::append(frameworkSearchPaths, m_settings->getFrameworkSearchPathsExpandedAndAbsolute());
	utility::append(frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	std::vector<std::string> compilerFlags;
	utility::append(compilerFlags, m_settings->getCompilerFlags());

	std::set<FilePath> indexedPaths = getIndexedPaths();
	std::set<FilePath> excludedPaths = getExcludedPaths();

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (cdbPath.exists())
	{
		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
			(clang::tooling::JSONCompilationDatabase::loadFromFile(cdbPath.str(), error, clang::tooling::JSONCommandLineSyntax::AutoDetect));
		if (!error.empty())
		{
			const std::string message = "Loading Clang compilation database failed with error: \"" + error + "\"";
			LOG_ERROR(message);
			MessageStatus(message, true).dispatch();
		}

		for (const clang::tooling::CompileCommand& command: cdb->getAllCompileCommands())
		{
			FilePath sourcePath = FilePath(command.Filename).canonical();
			if (!sourcePath.isAbsolute())
			{
				sourcePath = FilePath(command.Directory + '/' + command.Filename).canonical();
			}

			if (filesToIndex->find(sourcePath) != filesToIndex->end() &&
				sourceFilePathsToIndex.find(sourcePath) != sourceFilePathsToIndex.end())
			{
				std::vector<std::string> currentCompilerFlags = compilerFlags;
				currentCompilerFlags.insert(currentCompilerFlags.end(), command.CommandLine.begin(), command.CommandLine.end());

				indexerCommands.push_back(std::make_shared<IndexerCommandCxxCdb>(
					sourcePath,
					indexedPaths,
					excludedPaths,
					FilePath(command.Directory),
					currentCompilerFlags,
					systemHeaderSearchPaths,
					frameworkSearchPaths,
					m_settings->getShouldApplyAnonymousTypedefTransformation()
				));

				filesToIndex->erase(sourcePath);
			}
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettingsCxx> SourceGroupCxxCdb::getSourceGroupSettingsCxx()
{
	return m_settings;
}

std::vector<FilePath> SourceGroupCxxCdb::getAllSourcePaths() const
{
	std::vector<FilePath> sourcePaths;

	FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (cdbPath.exists())
	{
		sourcePaths = IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath);
	}

	return sourcePaths;
}
