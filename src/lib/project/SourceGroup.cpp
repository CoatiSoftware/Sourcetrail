#include "SourceGroup.h"

#include "MemoryIndexerCommandProvider.h"
#include "SourceGroupSettings.h"
#include "FilePath.h"

std::shared_ptr<IndexerCommandProvider> SourceGroup::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	return std::make_shared<MemoryIndexerCommandProvider>(getIndexerCommands(filesToIndex));
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
