#ifndef SOURCE_GROUP_H
#define SOURCE_GROUP_H

#include <memory>
#include <set>
#include <vector>

#include "LanguageType.h"
#include "SourceGroupStatusType.h"
#include "SourceGroupType.h"

class DialogView;
class FilePath;
class FilePathFilter;
class IndexerCommand;
class IndexerCommandProvider;
class SourceGroupSettings;
class StorageProvider;
class Task;

class SourceGroup
{
public:
	virtual ~SourceGroup() = default;

	virtual bool prepareIndexing();
	virtual bool allowsPartialClearing() const;

	virtual std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const = 0;
	virtual std::set<FilePath> getAllSourceFilePaths() const = 0;
	virtual std::shared_ptr<IndexerCommandProvider> getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const;
	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const = 0;
	virtual std::shared_ptr<Task> getPreIndexTask(
		std::shared_ptr<StorageProvider> storageProvider, std::shared_ptr<DialogView> dialogView) const;

	SourceGroupType getType() const;
	LanguageType getLanguage() const;
	SourceGroupStatusType getStatus() const;
	std::set<FilePath> filterToContainedSourceFilePath(const std::set<FilePath>& staticSourceFilePaths) const;
	bool containsSourceFilePath(const FilePath& sourceFilePath) const;

protected:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;
	virtual std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const = 0;

	std::set<FilePath> filterToContainedFilePaths(
		const std::set<FilePath>& filePaths,
		const std::set<FilePath>& indexedFilePaths,
		const std::set<FilePath>& indexedFileOrDirectoryPaths,
		const std::vector<FilePathFilter>& excludeFilters) const;
};

#endif // SOURCE_GROUP_H
