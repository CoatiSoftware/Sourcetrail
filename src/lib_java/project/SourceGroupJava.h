#ifndef SOURCE_GROUP_JAVA_H
#define SOURCE_GROUP_JAVA_H

#include <memory>
#include <set>
#include <vector>

#include "SourceGroup.h"

class SourceGroupJava: public SourceGroup
{
public:
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const RefreshInfo& info) const override;

private:
	virtual std::vector<FilePath> getAllSourcePaths() const = 0;
	virtual std::vector<FilePath> doGetClassPath() const = 0;

	std::vector<FilePath> getClassPath() const;
};

#endif	  // SOURCE_GROUP_JAVA_H
