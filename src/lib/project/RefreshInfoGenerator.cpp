#include "project/RefreshInfoGenerator.h"

#include "data/storage/PersistentStorage.h"
#include "project/RefreshInfo.h"
#include "project/SourceGroup.h"
#include "settings/SourceGroupStatusType.h"
#include "utility/file/FileInfo.h"
#include "utility/file/FileSystem.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

RefreshInfo RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
	const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups, std::shared_ptr<const PersistentStorage> storage)
{
	const std::set<FilePath> allSourceFilePathsFromSourcegroups = getAllSourceFilePaths(sourceGroups);

	std::set<FilePath> unchangedFilePaths;
	std::set<FilePath> changedFilePaths;

	{
		const std::vector<FileInfo> fileInfosFromStorage = storage->getFileInfoForAllIndexedFiles();

		std::set<FilePath> alreadyIndexedPaths;
		{
			const std::set<FilePath> filePathsFromStorage = utility::toSet(utility::convert<FileInfo, FilePath>(
				fileInfosFromStorage, [](const FileInfo& info) { return info.path; }
			));

			for (std::shared_ptr<SourceGroup> sourceGroup : sourceGroups)
			{
				utility::append(alreadyIndexedPaths, sourceGroup->filterToContainedFilePaths(filePathsFromStorage));
			}
		}

		// checking source and header files
		for (const FileInfo& info : fileInfosFromStorage)
		{
			if (alreadyIndexedPaths.find(info.path) != alreadyIndexedPaths.end() && info.path.exists())
			{
				if (didFileChange(info, storage))
				{
					changedFilePaths.insert(info.path);
				}
				else
				{
					unchangedFilePaths.insert(info.path);
				}
			}
			else // file has been removed
			{
				changedFilePaths.insert(info.path);
			}
		}
	}

	std::set<FilePath> filesToClear = changedFilePaths;

	// handle referencing paths
	utility::append(filesToClear, storage->getReferencing(changedFilePaths));

	// handle referenced paths
	std::set<FilePath> staticSourceFiles = allSourceFilePathsFromSourcegroups;
	for (const FilePath& path : changedFilePaths)
	{
		staticSourceFiles.erase(path);
	}

	const std::set<FilePath> staticReferencedFilePaths = storage->getReferenced(staticSourceFiles);
	const std::set<FilePath> dynamicReferencedFilePaths = storage->getReferenced(changedFilePaths);

	for (const FilePath& path : dynamicReferencedFilePaths)
	{
		if (staticReferencedFilePaths.find(path) == staticReferencedFilePaths.end() &&
			staticSourceFiles.find(path) == staticSourceFiles.end())
		{
			// file may not be referenced anymore and will be reindexed if still needed
			filesToClear.insert(path);
		}
	}

	for (const FilePath& path : unchangedFilePaths)
	{
		staticSourceFiles.erase(path);
	}

	const std::set<FilePath> filesToAdd = staticSourceFiles;

	std::set<FilePath> staticSourceFilePaths;
	for (const FilePath& path : allSourceFilePathsFromSourcegroups)
	{
		if (filesToClear.find(path) == filesToClear.end() && filesToAdd.find(path) == filesToAdd.end())
		{
			staticSourceFilePaths.insert(path);
		}
	}

	RefreshInfo info;
	info.mode = REFRESH_UPDATED_FILES;
	info.filesToClear = filesToClear;

	for (const std::shared_ptr<SourceGroup>& sourceGroup : sourceGroups)
	{
		if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
		{
			utility::append(info.filesToIndex, sourceGroup->filterToContainedSourceFilePath(staticSourceFilePaths));
		}
	}

	return info;
}

RefreshInfo RefreshInfoGenerator::getRefreshInfoForIncompleteFiles(const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups, std::shared_ptr<const PersistentStorage> storage)
{
	RefreshInfo info = getRefreshInfoForUpdatedFiles(sourceGroups, storage);
	info.mode = REFRESH_UPDATED_AND_INCOMPLETE_FILES;

	std::set<FilePath> incompleteFiles;
	for (const FilePath& path : storage->getIncompleteFiles())
	{
		if (info.filesToClear.find(path) == info.filesToClear.end())
		{
			incompleteFiles.insert(path);
		}
	}

	if (!incompleteFiles.empty())
	{
		utility::append(incompleteFiles, storage->getReferencing(incompleteFiles));

		std::set<FilePath> staticSourceFilePaths = getAllSourceFilePaths(sourceGroups);
		for (const FilePath& path : incompleteFiles)
		{
			staticSourceFilePaths.erase(path);

			if (storage->getFilePathIndexed(path))
			{
				info.filesToClear.insert(path);
			}
			else
			{
				info.nonIndexedFilesToClear.insert(path);
			}
		}

		for (const std::shared_ptr<const SourceGroup>& sourceGroup : sourceGroups)
		{
			if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
			{
				utility::append(info.filesToIndex, sourceGroup->filterToContainedSourceFilePath(staticSourceFilePaths));
			}
		}
	}

	return info;
}

RefreshInfo RefreshInfoGenerator::getRefreshInfoForAllFiles(const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups)
{
	RefreshInfo info;
	info.mode = REFRESH_ALL_FILES;
	info.filesToIndex = getAllSourceFilePaths(sourceGroups);
	return info;
}

std::set<FilePath> RefreshInfoGenerator::getAllSourceFilePaths(const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups)
{
	std::set<FilePath> allSourceFilePaths;

	for (const std::shared_ptr<const SourceGroup>& sourceGroup : sourceGroups)
	{
		if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
		{
			utility::append(allSourceFilePaths, sourceGroup->getAllSourceFilePaths());
		}
	}

	return allSourceFilePaths;
}

bool RefreshInfoGenerator::didFileChange(const FileInfo& info, std::shared_ptr<const PersistentStorage> storage)
{
	FileInfo diskFileInfo = FileSystem::getFileInfoForPath(info.path);
	if (diskFileInfo.lastWriteTime > info.lastWriteTime)
	{
		std::shared_ptr<TextAccess> storedFileContent = storage->getFileContent(info.path);
		std::shared_ptr<TextAccess> diskFileContent = TextAccess::createFromFile(diskFileInfo.path);

		const std::vector<std::string>& diskFileLines = diskFileContent->getAllLines();
		const std::vector<std::string>& storedFileLines = storedFileContent->getAllLines();

		if (diskFileLines.size() == storedFileLines.size())
		{
			for (size_t i = 0; i < diskFileLines.size(); i++)
			{
				if (diskFileLines[i] != storedFileLines[i])
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
	return false;
}
