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

	virtual bool prepareIndexing() override;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

protected:
	virtual std::vector<FilePath> doGetClassPath() const;

private:
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() = 0;
	virtual std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const = 0;
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	virtual std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;
	bool prepareJavaEnvironment();

	std::vector<FilePath> getClassPath() const;
	std::set<FilePath> fetchRootDirectories() const;
};

#endif // SOURCE_GROUP_JAVA_H
