#include "SourceGroupCustomCommand.h"

#include "FileManager.h"
#include "IndexerCommandCustom.h"
#include "ProjectSettings.h"
#include "RefreshInfo.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SqliteIndexStorage.h"
#include "utility.h"

SourceGroupCustomCommand::SourceGroupCustomCommand(
	std::shared_ptr<SourceGroupSettingsCustomCommand> settings)
	: m_settings(settings)
{
}

bool SourceGroupCustomCommand::allowsPartialClearing() const
{
	return false;
}

std::set<FilePath> SourceGroupCustomCommand::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(m_settings->getSourcePathsExpandedAndAbsolute()),
		m_settings->getExcludeFiltersExpandedAndAbsolute());
}

std::set<FilePath> SourceGroupCustomCommand::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludeFiltersExpandedAndAbsolute(),
		m_settings->getSourceExtensions());
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCustomCommand::getIndexerCommands(
	const RefreshInfo& info) const
{
	std::vector<std::wstring> parts;
	{
		const std::wstring customCommand = m_settings->getCustomCommand();

		std::wstring tmp;
		int quoteCount = 0;
		bool inQuote = false;
		// handle quoting. tokens can be surrounded by double quotes
		// "hello world". three consecutive double quotes represent
		// the quote character itself.
		for (int i = 0; i < customCommand.size(); ++i)
		{
			if (customCommand.at(i) == L'"')
			{
				++quoteCount;
				if (quoteCount == 3)	// third consecutive quote
				{
					quoteCount = 0;
					tmp += customCommand.at(i);
				}
				continue;
			}
			if (quoteCount)
			{
				if (quoteCount == 1)
				{
					inQuote = !inQuote;
				}
				quoteCount = 0;
			}
			if (!inQuote && customCommand.at(i) == L' ')
			{
				if (!tmp.empty())
				{
					parts.push_back(tmp);
					tmp.clear();
				}
			}
			else
			{
				tmp += customCommand.at(i);
			}
		}
		if (!tmp.empty())
		{
			parts.push_back(tmp);
		}
	}

	if (parts.empty())
	{
		return {};
	}

	const std::wstring command = parts.front();
	std::vector<std::wstring> args;
	for (size_t i = 1; i < parts.size(); i++)
	{
		args.push_back(parts[i]);
	}

	const bool runInParallel = m_settings->getRunInParallel();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (info.filesToIndex.find(sourcePath) != info.filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCustom>(
				command,
				args,
				m_settings->getProjectSettings()->getProjectFilePath(),
				m_settings->getProjectSettings()->getTempDBFilePath(),
				std::to_wstring(SqliteIndexStorage::getStorageVersion()),
				sourcePath,
				runInParallel));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupCustomCommand::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCustomCommand::getSourceGroupSettings() const
{
	return m_settings;
}
