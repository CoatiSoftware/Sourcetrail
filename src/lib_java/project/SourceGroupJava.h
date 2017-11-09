#ifndef SOURCE_GROUP_JAVA_H
#define SOURCE_GROUP_JAVA_H

#include <memory>
#include <set>
#include <vector>

#include "project/SourceGroup.h"

class SourceGroupSettingsJava;

class SourceGroupJava: public SourceGroup
{
public:
	SourceGroupJava();
	virtual ~SourceGroupJava();

	virtual bool prepareIndexing();

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
		const std::set<FilePath>& filesToIndex, bool fullRefresh);

protected:
	virtual std::vector<FilePath> doGetClassPath();

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() = 0;

	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings();
	bool prepareJavaEnvironment();

	std::vector<FilePath> getClassPath();
	std::set<FilePath> fetchRootDirectories();
};

#endif // SOURCE_GROUP_JAVA_H
