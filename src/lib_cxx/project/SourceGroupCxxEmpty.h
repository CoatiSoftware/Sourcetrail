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

	virtual SourceGroupType getType() const;

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(
		std::set<FilePath>* filesToIndex, bool fullRefresh);

private:
	virtual std::shared_ptr<SourceGroupSettingsCxx> getSourceGroupSettingsCxx();
	virtual std::vector<FilePath> getAllSourcePaths() const;

	std::shared_ptr<SourceGroupSettingsCxxEmpty> m_settings;
};

#endif // SOURCE_GROUP_CXX_EMPTY_H
