#include "project/SourceGroupCxxCdb.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"
#include "Application.h"

SourceGroupCxxCdb::SourceGroupCxxCdb(std::shared_ptr<SourceGroupSettingsCxxCdb> settings)
	: m_settings(settings)
{
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

std::set<FilePath> SourceGroupCxxCdb::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;

	const std::set<FilePath> indexedPaths = getIndexedPaths();
	const std::vector<FilePathFilter> excludeFilters = m_settings->getExcludeFiltersExpandedAndAbsolute();

	for (const FilePath& filePath : filePaths)
	{
		bool isInIndexedPaths = false;
		for (const FilePath& indexedPath : indexedPaths)
		{
			if (indexedPath == filePath || indexedPath.contains(filePath))
			{
				isInIndexedPaths = true;
				break;
			}
		}

		if (isInIndexedPaths)
		{
			for (const FilePathFilter& excludeFilter : excludeFilters)
			{
				if (excludeFilter.isMatching(filePath))
				{
					isInIndexedPaths = false;
					break;
				}
			}
		}

		if (isInIndexedPaths)
		{
			containedFilePaths.insert(filePath);
		}
	}

	return containedFilePaths;
}

std::set<FilePath> SourceGroupCxxCdb::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;

	const std::vector<FilePathFilter> excludeFilters = m_settings->getExcludeFiltersExpandedAndAbsolute();
	const FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();

	if (!cdbPath.empty() && cdbPath.exists())
	{
		for (const FilePath& path : IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath))
		{
			bool excluded = false;
			for (const FilePathFilter& filter : excludeFilters)
			{
				if (filter.isMatching(path))
				{
					excluded = true;
					break;
				}
			}

			if (!excluded &&  path.exists())
			{
				sourceFilePaths.insert(path);
			}
		}
	}

	return sourceFilePaths;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCdb::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	std::vector<FilePath> systemHeaderSearchPaths;
	utility::append(systemHeaderSearchPaths, m_settings->getHeaderSearchPathsExpandedAndAbsolute());
	utility::append(systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	std::vector<FilePath> frameworkSearchPaths;
	utility::append(frameworkSearchPaths, m_settings->getFrameworkSearchPathsExpandedAndAbsolute());
	utility::append(frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	const std::vector<std::wstring> compilerFlags = m_settings->getCompilerFlags();

	const std::set<FilePath> indexedPaths = getIndexedPaths();
	const std::set<FilePathFilter> excludeFilters = utility::toSet(m_settings->getExcludeFiltersExpandedAndAbsolute());

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
					excludeFilters,
					std::set<FilePathFilter>(),
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

std::shared_ptr<SourceGroupSettings> SourceGroupCxxCdb::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxCdb::getSourceGroupSettings() const
{
	return m_settings;
}

std::set<FilePath> SourceGroupCxxCdb::getIndexedPaths() const
{
	std::set<FilePath> indexedPaths;
	utility::append(indexedPaths, getAllSourceFilePaths());
	utility::append(indexedPaths, utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute()));
	return indexedPaths;
}
