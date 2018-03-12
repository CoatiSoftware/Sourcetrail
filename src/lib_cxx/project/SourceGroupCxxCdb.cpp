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

bool SourceGroupCxxCdb::prepareIndexing()
{
	FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (!cdbPath.empty() && !cdbPath.exists())
	{
		MessageStatus(L"Can't refresh project").dispatch();

		if (std::shared_ptr<Application> application = Application::getInstance())
		{
			if (application->hasGUI())
			{
				application->handleDialog(
					L"Can't refresh. The compilation database of the project does not exist anymore: " + cdbPath.wstr(),
					{ L"Ok" }
				);
			}
		}
		return false;
	}
	return true;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCdb::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
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

	const std::vector<std::wstring> compilerFlags = m_settings->getCompilerFlags();

	std::set<FilePath> indexedPaths = getIndexedPaths();
	std::set<FilePathFilter> excludeFilters = getExcludeFilters();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (cdbPath.exists())
	{
		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb =
			std::shared_ptr<clang::tooling::JSONCompilationDatabase>(
				clang::tooling::JSONCompilationDatabase::loadFromFile(utility::encodeToUtf8(cdbPath.wstr()),
				error,
				clang::tooling::JSONCommandLineSyntax::AutoDetect
			)
		);

		if (!error.empty())
		{
			const std::wstring message = L"Loading Clang compilation database failed with error: \"" + utility::decodeFromUtf8(error) + L"\"";
			LOG_ERROR(message);
			MessageStatus(message, true).dispatch();
		}

		const std::set<FilePath>& sourceFilePaths = getAllSourceFilePaths();

		for (const clang::tooling::CompileCommand& command: cdb->getAllCompileCommands())
		{
			FilePath sourcePath = FilePath(utility::decodeFromUtf8(command.Filename)).makeCanonical();
			if (!sourcePath.isAbsolute())
			{
				sourcePath = FilePath(utility::decodeFromUtf8(command.Directory + '/' + command.Filename)).makeCanonical();
			}

			if (filesToIndex.find(sourcePath) != filesToIndex.end() &&
				sourceFilePaths.find(sourcePath) != sourceFilePaths.end())
			{
				indexerCommands.push_back(std::make_shared<IndexerCommandCxxCdb>(
					sourcePath,
					indexedPaths,
					getExcludeFilters(),
					FilePath(utility::decodeFromUtf8(command.Directory)),
					utility::concat(
						utility::convert<std::string, std::wstring>(command.CommandLine, [](const std::string& arg) { return utility::decodeFromUtf8(arg); }), 
						compilerFlags
					),
					systemHeaderSearchPaths,
					frameworkSearchPaths
				));
			}
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettingsCxx> SourceGroupCxxCdb::getSourceGroupSettingsCxx()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettingsCxx> SourceGroupCxxCdb::getSourceGroupSettingsCxx() const
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
