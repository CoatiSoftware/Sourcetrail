#ifndef SOURCE_GROUP_CXX_EMPTY_H
#define SOURCE_GROUP_CXX_EMPTY_H

#include <memory>
#include <set>

#include "SourceGroup.h"

class SourceGroupSettingsCxx;

class SourceGroupCxxEmpty: public SourceGroup
{
public:
	SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxx> settings);

	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override;
	std::set<FilePath> getAllSourceFilePaths() const override;
	std::shared_ptr<IndexerCommandProvider> getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const override;
	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;
	std::shared_ptr<Task> getPreIndexTask(std::shared_ptr<DialogView> dialogView) const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;
	std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override;
	std::vector<std::wstring> getBaseCompilerFlags() const;

	std::shared_ptr<SourceGroupSettingsCxx> m_settings;
};

#endif // SOURCE_GROUP_CXX_EMPTY_H
