#ifndef SOURCE_GROUP_CXX_EMPTY_H
#define SOURCE_GROUP_CXX_EMPTY_H

#include <memory>
#include <set>

#include "project/SourceGroupCxx.h"
#include "settings/SourceGroupSettingsCxxEmpty.h"

class SourceGroupCxxEmpty: public SourceGroupCxx
{
public:
	SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxxEmpty> settings);
	virtual ~SourceGroupCxxEmpty();

	virtual SourceGroupType getType() const override;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override;

private:
	virtual std::shared_ptr<SourceGroupSettingsCxx> getSourceGroupSettingsCxx() override;
	virtual std::shared_ptr<const SourceGroupSettingsCxx> getSourceGroupSettingsCxx() const override;
	virtual std::vector<FilePath> getAllSourcePaths() const override;

	std::shared_ptr<SourceGroupSettingsCxxEmpty> m_settings;
};

#endif // SOURCE_GROUP_CXX_EMPTY_H
