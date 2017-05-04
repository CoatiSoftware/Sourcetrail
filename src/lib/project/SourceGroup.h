#ifndef SOURCE_GROUP_H
#define SOURCE_GROUP_H

#include <memory>
#include <set>
#include <vector>

#include "settings/LanguageType.h"
#include "settings/SourceGroupType.h"

class FilePath;
class IndexerCommand;

class SourceGroup
{
public:
	virtual ~SourceGroup();

	virtual SourceGroupType getType() const = 0;
	LanguageType getLanguage() const;

	virtual bool prepareRefresh();
	virtual bool prepareIndexing();

	virtual void fetchAllSourceFilePaths() = 0;
	void fetchSourceFilePathsToIndex(const std::set<FilePath>& staticSourceFilePaths);

	std::set<FilePath> getAllSourceFilePaths() const;
	std::set<FilePath> getSourceFilePathsToIndex() const;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const bool fullRefresh) = 0;

protected:
	std::set<FilePath> m_allSourceFilePaths;
	std::set<FilePath> m_sourceFilePathsToIndex;
};

#endif // SOURCE_GROUP_H
