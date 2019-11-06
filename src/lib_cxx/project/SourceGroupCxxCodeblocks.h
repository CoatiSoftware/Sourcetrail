#ifndef SOURCE_GROUP_CXX_CODEBLOCKS_H
#define SOURCE_GROUP_CXX_CODEBLOCKS_H

#include <memory>
#include <set>

#include "SourceGroup.h"

class SourceGroupSettingsCxxCodeblocks;

class SourceGroupCxxCodeblocks
	: public SourceGroup
{
public:
	SourceGroupCxxCodeblocks(std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings);

	bool prepareIndexing() override;
	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::shared_ptr<IndexerCommandProvider> getIndexerCommandProvider(const RefreshInfo& info) const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const RefreshInfo& info) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> m_settings;
};

#endif // SOURCE_GROUP_CXX_CODEBLOCKS_H
