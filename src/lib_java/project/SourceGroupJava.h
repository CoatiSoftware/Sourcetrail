#ifndef SOURCE_GROUP_JAVA_H
#define SOURCE_GROUP_JAVA_H

#include <memory>
#include <set>
#include <vector>

#include "SourceGroup.h"

class SourceGroupSettingsJava;

class SourceGroupJava: public SourceGroup
{
public:
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

private:
	virtual std::vector<FilePath> getAllSourcePaths() const = 0;
	virtual std::vector<FilePath> doGetClassPath() const = 0;
	virtual std::shared_ptr<SourceGroupSettingsJava> getSourceGroupSettingsJava() = 0;
	virtual std::shared_ptr<const SourceGroupSettingsJava> getSourceGroupSettingsJava() const = 0;
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::vector<FilePath> getClassPath() const;
};

#endif // SOURCE_GROUP_JAVA_H
