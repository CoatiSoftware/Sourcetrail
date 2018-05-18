#ifndef SOURCE_GROUP_H
#define SOURCE_GROUP_H

#include <memory>
#include <set>
#include <vector>

class IndexerCommand;
class FilePath;
class SourceGroupSettings;
enum LanguageType;
enum SourceGroupStatusType;
enum SourceGroupType;

class SourceGroup
{
public:
	virtual ~SourceGroup() = default;

	virtual bool prepareIndexing();
	virtual std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const = 0;
	virtual std::set<FilePath> getAllSourceFilePaths() const = 0;
	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const = 0;

	SourceGroupType getType() const;
	LanguageType getLanguage() const;
	SourceGroupStatusType getStatus() const;
	std::set<FilePath> filterToContainedSourceFilePath(const std::set<FilePath>& staticSourceFilePaths) const;
	bool containsSourceFilePath(const FilePath& sourceFilePath) const;

protected:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;
	virtual std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const = 0;
};

#endif // SOURCE_GROUP_H
