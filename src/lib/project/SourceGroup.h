#ifndef SOURCE_GROUP_H
#define SOURCE_GROUP_H

#include <memory>
#include <set>
#include <vector>

#include "settings/LanguageType.h"
#include "settings/SourceGroupStatusType.h"
#include "settings/SourceGroupType.h"
#include "utility/file/FilePath.h"
#include "utility/file/FilePathFilter.h"

class IndexerCommand;
class SourceGroupSettings;

class SourceGroup
{
public:
	virtual ~SourceGroup();

	virtual SourceGroupType getType() const = 0;
	SourceGroupStatusType getStatus() const;
	LanguageType getLanguage() const;

	virtual bool prepareIndexing();
	void fetchAllSourceFilePaths();

	std::set<FilePath> getIndexedPaths() const;
	std::set<FilePathFilter> getExcludeFilters() const;
	std::set<FilePath> getAllSourceFilePaths() const;
	std::set<FilePath> getSourceFilePathsToIndex(const std::set<FilePath>& staticSourceFilePaths) const;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const = 0;

	std::set<FilePath> m_allSourceFilePaths;

private:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;
	virtual std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const = 0;
	virtual std::vector<FilePath> getAllSourcePaths() const = 0;

	std::set<FilePath> findAndAddSymlinkedDirectories(const std::vector<FilePath>& paths) const;
};

#endif // SOURCE_GROUP_H
