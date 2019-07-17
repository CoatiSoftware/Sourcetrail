#include "SourceGroupCxxCdb.h"

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/JSONCompilationDatabase.h>

#include "Application.h"
#include "ApplicationSettings.h"
#include "CxxIndexerCommandProvider.h"
#include "IndexerCommandCxx.h"
#include "logging.h"
#include "MessageStatus.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "utility.h"

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
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		getAllSourceFilePaths(),
		utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute()
	);
}

std::set<FilePath> SourceGroupCxxCdb::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;

	const std::vector<FilePathFilter> excludeFilters = m_settings->getExcludeFiltersExpandedAndAbsolute();
	const FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();

	if (!cdbPath.empty() && cdbPath.exists())
	{
		for (const FilePath& path : IndexerCommandCxx::getSourceFilesFromCDB(cdbPath))
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

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxCdb::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	std::shared_ptr<CxxIndexerCommandProvider> provider = std::make_shared<CxxIndexerCommandProvider>();

	const FilePath cdbPath = m_settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (cdbPath.exists())
	{
		std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

		std::vector<std::wstring> compilerFlags;
		{
			utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(
				utility::concat(m_settings->getHeaderSearchPathsExpandedAndAbsolute(), appSettings->getHeaderSearchPathsExpanded())));

			utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(
				utility::concat(m_settings->getFrameworkSearchPathsExpandedAndAbsolute(), appSettings->getFrameworkSearchPathsExpanded())));

			utility::append(compilerFlags, m_settings->getCompilerFlags());
		}

		const std::set<FilePath> indexedHeaderPaths = utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute());
		const std::set<FilePathFilter> excludeFilters = utility::toSet(m_settings->getExcludeFiltersExpandedAndAbsolute());

		std::string error;
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>(
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
		if (cdb)
		{
			for (const clang::tooling::CompileCommand& command: cdb->getAllCompileCommands())
			{
				FilePath sourcePath = FilePath(utility::decodeFromUtf8(command.Filename)).makeCanonical();
				if (!sourcePath.isAbsolute())
				{
					sourcePath = FilePath(utility::decodeFromUtf8(command.Directory + '/' + command.Filename)).makeCanonical();
				}
				if (!sourcePath.isAbsolute())
				{
					sourcePath = cdbPath.getParentDirectory().getConcatenated(sourcePath).makeCanonical();
				}

				if (filesToIndex.find(sourcePath) != filesToIndex.end() &&
					sourceFilePaths.find(sourcePath) != sourceFilePaths.end())
				{
					std::vector<std::wstring> mergedCompilerFlags;
					mergedCompilerFlags.reserve(compilerFlags.size() + command.CommandLine.size());
					for (const std::string& arg : command.CommandLine)
					{
						mergedCompilerFlags.emplace_back(utility::decodeFromUtf8(arg));
					}
					mergedCompilerFlags.insert(mergedCompilerFlags.end(), compilerFlags.begin(), compilerFlags.end());

					provider->addCommand(std::make_shared<IndexerCommandCxx>(
						sourcePath,
						utility::concat(indexedHeaderPaths, { sourcePath }),
						excludeFilters,
						std::set<FilePathFilter>(),
						FilePath(utility::decodeFromUtf8(command.Directory)),
						mergedCompilerFlags
					));
				}
			}
		}
	}

	provider->logStats();

	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxCdb::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	return getIndexerCommandProvider(filesToIndex)->consumeAllCommands();
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxCdb::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxCdb::getSourceGroupSettings() const
{
	return m_settings;
}
