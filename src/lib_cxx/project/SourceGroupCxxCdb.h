#ifndef SOURCE_GROUP_CXX_CDB_H
#define SOURCE_GROUP_CXX_CDB_H

#include <memory>
#include <vector>
#include <set>

#include "project/SourceGroup.h"

class SourceGroupSettingsCxxCdb;

class SourceGroupCxxCdb: public SourceGroup
{
public:
	SourceGroupCxxCdb(std::shared_ptr<SourceGroupSettingsCxxCdb> settings);

	bool prepareIndexing() override;
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;
	std::set<FilePath> getIndexedPaths() const;

	std::shared_ptr<SourceGroupSettingsCxxCdb> m_settings;
};

#endif // SOURCE_GROUP_CXX_CDB_H
