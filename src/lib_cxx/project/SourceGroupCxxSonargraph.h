#ifndef SOURCE_GROUP_CXX_SONARGRAPH_H
#define SOURCE_GROUP_CXX_SONARGRAPH_H

#include <memory>
#include <set>

#include "project/SourceGroup.h"

class SourceGroupSettingsCxxSonargraph;

class SourceGroupCxxSonargraph
	: public SourceGroup
{
public:
	SourceGroupCxxSonargraph(std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings);

	bool prepareIndexing() override;
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsCxxSonargraph> m_settings;
};

#endif // SOURCE_GROUP_CXX_SONARGRAPH_H
