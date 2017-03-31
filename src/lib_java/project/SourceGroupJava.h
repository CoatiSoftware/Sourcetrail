#ifndef SOURCE_GROUP_JAVA_H
#define SOURCE_GROUP_JAVA_H

#include <memory>
#include <set>

#include "settings/SourceGroupSettingsJava.h"
#include "project/SourceGroup.h"
#include "utility/file/FileManager.h"

class SourceGroupJava: public SourceGroup
{
public:
	SourceGroupJava(std::shared_ptr<SourceGroupSettingsJava> settings);
	virtual ~SourceGroupJava();

	virtual SourceGroupType getType() const;

	virtual bool prepareIndexing();

	virtual void fetchAllSourceFilePaths();

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const bool fullRefresh);

private:
	bool prepareJavaEnvironment();
	bool prepareMavenData();

	std::vector<FilePath> getClassPath();
	std::set<FilePath> fetchRootDirectories();

	std::shared_ptr<SourceGroupSettingsJava> m_settings;
};

#endif // SOURCE_GROUP_JAVA_H
