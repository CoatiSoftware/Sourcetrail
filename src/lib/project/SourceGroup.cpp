#include "project/SourceGroup.h"

#include "utility/file/FilePath.h"

SourceGroup::~SourceGroup()
{
}

LanguageType SourceGroup::getLanguage() const
{
	return getLanguageTypeForSourceGroupType(getType());
}

bool SourceGroup::prepareRefresh()
{
	return true;
}

bool SourceGroup::prepareIndexing()
{
	return true;
}

void SourceGroup::fetchSourceFilePathsToIndex(const std::set<FilePath>& staticSourceFilePaths)
{
	for (const FilePath& sourceFilePath: m_allSourceFilePaths)
	{
		if (staticSourceFilePaths.find(sourceFilePath) == staticSourceFilePaths.end())
		{
			m_sourceFilePathsToIndex.insert(sourceFilePath);
		}
	}
}

std::set<FilePath> SourceGroup::getAllSourceFilePaths() const
{
	return m_allSourceFilePaths;
}

std::set<FilePath> SourceGroup::getSourceFilePathsToIndex() const
{
	return m_sourceFilePathsToIndex;
}
