#include "RefreshInfoGenerator.h"

#include "FileInfo.h"
#include "FileSystem.h"
#include "PersistentStorage.h"
#include "RefreshInfo.h"
#include "SourceGroup.h"
#include "SourceGroupStatusType.h"
#include "TextAccess.h"
#include "utility.h"

RefreshInfo RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
	const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups,
	std::shared_ptr<const PersistentStorage> storage)
{
	// 1) Divide filepaths that are already known by the storage to "unchanged and indexed",
	// "unchanged and non-indexed" and "changed"
	std::set<FilePath> unchangedIndexedFilePaths;
	std::set<FilePath> unchangedNonindexedFilePaths;
	std::set<FilePath> changedFilePaths;

	{
		const std::vector<FileInfo> fileInfosFromStorage = storage->getFileInfoForAllFiles();

		std::set<FilePath> alreadyKnownPaths;
		{
			const std::set<FilePath> filePathsFromStorage = utility::toSet(
				utility::convert<FileInfo, FilePath>(
					fileInfosFromStorage, [](const FileInfo& info) { return info.path; }));

			for (std::shared_ptr<SourceGroup> sourceGroup: sourceGroups)
			{
				if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
				{
					utility::append(
						alreadyKnownPaths,
						sourceGroup->filterToContainedFilePaths(filePathsFromStorage));
				}
			}
		}

		// checking source and header files
		for (const FileInfo& info: fileInfosFromStorage)
		{
			if (alreadyKnownPaths.find(info.path) != alreadyKnownPaths.end() && info.path.exists())
			{
				if (storage->getFilePathIndexed(info.path))
				{
					if (didFileChange(info, storage))
					{
						changedFilePaths.insert(info.path);
					}
					else
					{
						unchangedIndexedFilePaths.insert(info.path);
					}
				}
				else
				{
					changedFilePaths.insert(info.path);
				}
			}
			else if (!storage->getFilePathIndexed(info.path) && !didFileChange(info, storage))
			{
				unchangedNonindexedFilePaths.insert(info.path);
			}
			else	// file has been removed
			{
				changedFilePaths.insert(info.path);
			}
		}
	}

	const std::set<FilePath> allSourceFilePathsFromSourcegroups = getAllSourceFilePaths(sourceGroups);

	// 2) Figure out which files need to be cleared
	// 2.1) Add all changed files
	std::set<FilePath> filesToClear = changedFilePaths;

	// 2.2) Add files that are reference the changed files
	utility::append(filesToClear, storage->getReferencing(changedFilePaths));

	// 2.3) Handle files that are referenced by the files that will be cleared. These will be
	// re-indexed on the fly. However, we do not
	//		need to clear files that are also referenced by unchanged source files, because
	//otherwise we will lose these connections.
	// 2.3.1) Get all source file paths that will not be cleared.
	// - Initially this list contains all source file paths the project would index right now.
	// - Then we remove all source files that will be cleared
	// - NOTE: Source files that are new to the project will part of this list, but won't result in
	// any referenced
	//   paths because they are not part of the DB. Source files that are new to the project but are
	//   already in the DB will be removed from this list if they have changed or reference changed
	//   files.
	std::set<FilePath> staticSourceFiles = allSourceFilePathsFromSourcegroups;
	for (const FilePath& path: filesToClear)
	{
		staticSourceFiles.erase(path);
	}

	// 2.3.2) Get sets of referenced files
	const std::set<FilePath> staticReferencedFilePaths = storage->getReferenced(staticSourceFiles);
	const std::set<FilePath> dynamicReferencedFilePaths = storage->getReferenced(filesToClear);

	// 2.3.3) Add "dynamicReferencedFilePaths" to "filesToClear" that are not refenced by static
	// paths, because these files may not be
	//        referenced anymore. If they still are, they will be re-added when encountered during
	//        re-indexing.
	for (const FilePath& path: dynamicReferencedFilePaths)
	{
		if (staticReferencedFilePaths.find(path) == staticReferencedFilePaths.end() &&
			staticSourceFiles.find(path) == staticSourceFiles.end())
		{
			filesToClear.insert(path);
		}
	}

	// 3) Figure out which files need to be indexed
	std::set<FilePath> filesToIndex;
	for (const FilePath& path: allSourceFilePathsFromSourcegroups)
	{
		if (filesToClear.find(path) != filesToClear.end() ||	// file will be cleared
			unchangedIndexedFilePaths.find(path) ==
				unchangedIndexedFilePaths.end())	// file has been changed or added
		{
			filesToIndex.insert(path);
		}
	}

	// 4) Store and return this information
	RefreshInfo info;
	info.mode = REFRESH_UPDATED_FILES;
	info.filesToIndex = filesToIndex;
	for (const FilePath fileToClear: filesToClear)
	{
		if (storage->getFilePathIndexed(fileToClear))
		{
			info.filesToClear.insert(fileToClear);
		}
		else
		{
			info.nonIndexedFilesToClear.insert(fileToClear);
		}
	}

	return info;
}

RefreshInfo RefreshInfoGenerator::getRefreshInfoForIncompleteFiles(
	const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups,
	std::shared_ptr<const PersistentStorage> storage)
{
	RefreshInfo info = getRefreshInfoForUpdatedFiles(sourceGroups, storage);
	info.mode = REFRESH_UPDATED_AND_INCOMPLETE_FILES;

	std::set<FilePath> incompleteFiles;
	{
		const std::set<FilePath> filesToClear = utility::concat(
			info.filesToClear, info.nonIndexedFilesToClear);
		for (const FilePath& path: storage->getIncompleteFiles())
		{
			if (filesToClear.find(path) == filesToClear.end())
			{
				incompleteFiles.insert(path);
			}
		}
	}

	if (!incompleteFiles.empty())
	{
		utility::append(incompleteFiles, storage->getReferencing(incompleteFiles));

		std::set<FilePath> staticSourceFilePaths = getAllSourceFilePaths(sourceGroups);
		for (const FilePath& path: incompleteFiles)
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

		for (const std::shared_ptr<const SourceGroup>& sourceGroup: sourceGroups)
		{
			if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
			{
				utility::append(
					info.filesToIndex,
					sourceGroup->filterToContainedSourceFilePath(staticSourceFilePaths));
			}
		}
	}

	return info;
}

RefreshInfo RefreshInfoGenerator::getRefreshInfoForAllFiles(
	const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups)
{
	RefreshInfo info;
	info.mode = REFRESH_ALL_FILES;
	info.filesToIndex = getAllSourceFilePaths(sourceGroups);
	return info;
}

std::set<FilePath> RefreshInfoGenerator::getAllSourceFilePaths(
	const std::vector<std::shared_ptr<SourceGroup>>& sourceGroups)
{
	std::set<FilePath> allSourceFilePaths;

	for (const std::shared_ptr<const SourceGroup>& sourceGroup: sourceGroups)
	{
		if (sourceGroup->getStatus() == SOURCE_GROUP_STATUS_ENABLED)
		{
			for (const FilePath& sourceFilePath: sourceGroup->getAllSourceFilePaths())
			{
				if (sourceFilePath.exists())
				{
					allSourceFilePaths.insert(sourceFilePath);
				}
			}
		}
	}

	return allSourceFilePaths;
}

bool RefreshInfoGenerator::didFileChange(
	const FileInfo& info, std::shared_ptr<const PersistentStorage> storage)
{
	FileInfo diskFileInfo = FileSystem::getFileInfoForPath(info.path);
	if (diskFileInfo.lastWriteTime > info.lastWriteTime)
	{
		if (!storage->hasContentForFile(info.path))
		{
			return true;
		}

		std::shared_ptr<TextAccess> storedFileContent = storage->getFileContent(info.path, false);
		std::shared_ptr<TextAccess> diskFileContent = TextAccess::createFromFile(diskFileInfo.path);

		const std::vector<std::string>& diskFileLines = diskFileContent->getAllLines();
		const std::vector<std::string>& storedFileLines = storedFileContent->getAllLines();

		if (diskFileLines.size() != storedFileLines.size())
		{
			return true;
		}

		for (size_t i = 0; i < diskFileLines.size(); i++)
		{
			if (diskFileLines[i] != storedFileLines[i])
			{
				return true;
			}
		}
		return false;
	}
	return false;
}
