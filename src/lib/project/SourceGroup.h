#ifndef SOURCE_GROUP_H
#define SOURCE_GROUP_H

#include <memory>
#include <set>
#include <vector>

#include "settings/LanguageType.h"
#include "settings/SourceGroupType.h"

class FilePath;
class IndexerCommand;
class SourceGroupSettings;

class SourceGroup
{
public:
	virtual ~SourceGroup();

	virtual SourceGroupType getType() const = 0;
	LanguageType getLanguage() const;

	virtual bool prepareRefresh();
	virtual bool prepareIndexing();

	void fetchAllSourceFilePaths();
	void fetchSourceFilePathsToIndex(const std::set<FilePath>& staticSourceFilePaths);

	std::set<FilePath> getAllSourceFilePaths() const;
	std::set<FilePath> getSourceFilePathsToIndex() const;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
		std::set<FilePath>* filesToIndex, bool fullRefresh) = 0;

protected:
	std::set<FilePath> getIndexedPaths();
	std::set<FilePath> getExcludedPaths();

	std::set<FilePath> m_allSourceFilePaths;
	std::set<FilePath> m_sourceFilePathsToIndex;

private:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;
	virtual std::vector<FilePath> getAllSourcePaths() const = 0;

	std::set<FilePath> findAndAddSymlinkedDirectories(const std::vector<FilePath>& paths);
};

#endif // SOURCE_GROUP_H
