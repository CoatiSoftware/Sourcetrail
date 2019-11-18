#ifndef SOURCE_GROUP_CUSTOM_COMMAND_H
#define SOURCE_GROUP_CUSTOM_COMMAND_H

#include <memory>
#include <set>
#include <vector>

#include "SourceGroup.h"

class SourceGroupSettingsCustomCommand;

class SourceGroupCustomCommand: public SourceGroup
{
public:
	SourceGroupCustomCommand(std::shared_ptr<SourceGroupSettingsCustomCommand> settings);

	bool allowsPartialClearing() const override;

	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const RefreshInfo& info) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;

	std::shared_ptr<SourceGroupSettingsCustomCommand> m_settings;
};

#endif	  // SOURCE_GROUP_CUSTOM_COMMAND_H
