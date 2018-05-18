#include "project/SourceGroupJava.h"

#include "data/indexer/IndexerCommandJava.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/file/FileManager.h"
#include "utility/logging/logging.h"

std::set<FilePath> SourceGroupJava::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::set<FilePath> containedFilePaths;
	const std::set<FilePath> allSourceFilePaths = getAllSourceFilePaths();
	for (const FilePath& filePath : filePaths)
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
		getSourceGroupSettingsJava()->getExcludeFiltersExpandedAndAbsolute(),
		getSourceGroupSettingsJava()->getSourceExtensions()
	);
	return fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	const std::string languageStandard = getSourceGroupSettingsJava()->getStandard();

	std::vector<FilePath> classPath = getClassPath();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandJava>(
				sourcePath, languageStandard, classPath
			));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupJava::getSourceGroupSettings()
{
	return getSourceGroupSettingsJava();
}

std::shared_ptr<const SourceGroupSettings> SourceGroupJava::getSourceGroupSettings() const
{
	return getSourceGroupSettingsJava();
}

std::vector<FilePath> SourceGroupJava::getClassPath() const
{
	LOG_INFO("Retrieving classpath for indexer commands");
	std::vector<FilePath> classPath = doGetClassPath();
	LOG_INFO("Found " + std::to_string(classPath.size()) + " paths for classpath.");
	return classPath;
}
