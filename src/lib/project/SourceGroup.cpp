#include "SourceGroup.h"

#include "FilePath.h"
#include "FilePathFilter.h"
#include "MemoryIndexerCommandProvider.h"
#include "ProjectSettings.h"
#include "SourceGroupSettings.h"
#include "TaskLambda.h"

std::shared_ptr<IndexerCommandProvider> SourceGroup::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	return std::make_shared<MemoryIndexerCommandProvider>(getIndexerCommands(filesToIndex));
}

std::shared_ptr<Task> SourceGroup::getPreIndexTask(
	std::shared_ptr<StorageProvider> storageProvider, std::shared_ptr<DialogView> dialogView) const
{
	return std::make_shared<TaskLambda>([]() {});
}

SourceGroupType SourceGroup::getType() const
{
	return getSourceGroupSettings()->getType();
}

LanguageType SourceGroup::getLanguage() const
{
	return getSourceGroupSettings()->getLanguage();
}

SourceGroupStatusType SourceGroup::getStatus() const
{
	return getSourceGroupSettings()->getStatus();
}

bool SourceGroup::prepareIndexing()
{
	return true;
}

bool SourceGroup::allowsPartialClearing() const
{
	return true;
}

std::set<FilePath> SourceGroup::filterToContainedSourceFilePath(const std::set<FilePath>& sourceFilePaths) const
{
	std::set<FilePath> filteredSourceFilePaths;
	for (const FilePath& sourceFilePath: getAllSourceFilePaths())
	{
		if (sourceFilePaths.find(sourceFilePath) == sourceFilePaths.end())
		{
			filteredSourceFilePaths.insert(sourceFilePath);
		}
	}
	return filteredSourceFilePaths;
}

bool SourceGroup::containsSourceFilePath(const FilePath& sourceFilePath) const
{
	return !filterToContainedSourceFilePath({ sourceFilePath }).empty();
}

std::set<FilePath> SourceGroup::filterToContainedFilePaths(
	const std::set<FilePath>& filePaths,
	const std::set<FilePath>& indexedFilePaths,
	const std::set<FilePath>& indexedFileOrDirectoryPaths,
	const std::vector<FilePathFilter>& excludeFilters) const
{
	std::set<FilePath> containedFilePaths;

	for (const FilePath& filePath : filePaths)
	{
		bool isInIndexedPaths = false;

		for (const FilePath& indexedFileOrDirectoryPath : indexedFileOrDirectoryPaths)
		{
			if (indexedFileOrDirectoryPath == filePath || indexedFileOrDirectoryPath.contains(filePath))
			{
				isInIndexedPaths = true;
				break;
			}
		}

		if (!isInIndexedPaths && indexedFilePaths.find(filePath) != indexedFilePaths.end())
		{
			isInIndexedPaths = true;
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
