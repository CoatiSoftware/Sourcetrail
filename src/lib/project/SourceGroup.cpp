#include "project/SourceGroup.h"

#include "settings/SourceGroupSettings.h"
#include "utility/file/FileManager.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/utility.h"

SourceGroup::~SourceGroup()
{
}

SourceGroupStatusType SourceGroup::getStatus() const
{
	return getSourceGroupSettings()->getStatus();
}

LanguageType SourceGroup::getLanguage() const
{
	return getLanguageTypeForSourceGroupType(getType());
}

bool SourceGroup::prepareIndexing()
{
	return true;
}

void SourceGroup::fetchAllSourceFilePaths()
{
	FileManager fileManager;
	fileManager.update(
		getAllSourcePaths(),
		getSourceGroupSettings()->getExcludeFiltersExpandedAndAbsolute(),
		getSourceGroupSettings()->getSourceExtensions()
	);
	m_allSourceFilePaths = fileManager.getAllSourceFilePaths();
}

std::set<FilePath> SourceGroup::getIndexedPaths() const
{
	return findAndAddSymlinkedDirectories(getSourceGroupSettings()->getSourcePathsExpandedAndAbsolute());
}

std::set<FilePathFilter> SourceGroup::getExcludeFilters() const
{
	return utility::toSet(getSourceGroupSettings()->getExcludeFiltersExpandedAndAbsolute());
}

std::set<FilePath> SourceGroup::getAllSourceFilePaths() const
{
	return m_allSourceFilePaths;
}

std::set<FilePath> SourceGroup::getSourceFilePathsToIndex(const std::set<FilePath>& staticSourceFilePaths) const
{
	std::set<FilePath> sourceFilePathsToIndex;
	for (const FilePath& sourceFilePath: m_allSourceFilePaths)
	{
		if (staticSourceFilePaths.find(sourceFilePath) == staticSourceFilePaths.end())
		{
			sourceFilePathsToIndex.insert(sourceFilePath);
		}
	}
	return sourceFilePathsToIndex;
}

std::set<FilePath> SourceGroup::findAndAddSymlinkedDirectories(const std::vector<FilePath>& paths) const
{
	std::set<FilePath> resultPaths;
	for (const FilePath& path: paths)
	{
		if (path.exists())
		{
			resultPaths.insert(path);
		}
	}

	std::set<FilePath> symLinkPaths = FileSystem::getSymLinkedDirectories(paths);
	resultPaths.insert(symLinkPaths.begin(), symLinkPaths.end());

	return resultPaths;
}
