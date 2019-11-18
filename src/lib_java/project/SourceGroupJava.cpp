#include "SourceGroupJava.h"

#include "FileManager.h"
#include "IndexerCommandJava.h"
#include "RefreshInfo.h"
#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "SourceGroupSettingsWithJavaStandard.h"
#include "SourceGroupSettingsWithSourceExtensions.h"
#include "logging.h"

std::set<FilePath> SourceGroupJava::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;
	const std::set<FilePath> allSourceFilePaths = getAllSourceFilePaths();
	for (const FilePath& filePath: filePaths)
	{
		if (allSourceFilePaths.find(filePath) != allSourceFilePaths.end())
		{
			containedFilePaths.insert(filePath);
		}
	}
	return containedFilePaths;
}

std::set<FilePath> SourceGroupJava::getAllSourceFilePaths() const
{
	FileManager fileManager;
	fileManager.update(
		getAllSourcePaths(),
		dynamic_cast<const SourceGroupSettingsWithExcludeFilters*>(getSourceGroupSettings().get())
			->getExcludeFiltersExpandedAndAbsolute(),
		dynamic_cast<const SourceGroupSettingsWithSourceExtensions*>(getSourceGroupSettings().get())
			->getSourceExtensions());
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(
	const RefreshInfo& info) const
{
	const std::wstring languageStandard = dynamic_cast<const SourceGroupSettingsWithJavaStandard*>(
											  getSourceGroupSettings().get())
											  ->getJavaStandard();

	std::vector<FilePath> classPath = getClassPath();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (info.filesToIndex.find(sourcePath) != info.filesToIndex.end())
		{
			indexerCommands.push_back(
				std::make_shared<IndexerCommandJava>(sourcePath, languageStandard, classPath));
		}
	}

	return indexerCommands;
}

std::vector<FilePath> SourceGroupJava::getClassPath() const
{
	LOG_INFO("Retrieving classpath for indexer commands");
	std::vector<FilePath> classPath = doGetClassPath();
	LOG_INFO("Found " + std::to_string(classPath.size()) + " paths for classpath.");
	return classPath;
}
