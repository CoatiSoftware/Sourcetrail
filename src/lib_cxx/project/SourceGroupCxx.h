#ifndef SOURCE_GROUP_CXX_H
#define SOURCE_GROUP_CXX_H

#include <memory>
#include <set>

#include "project/SourceGroup.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "utility/file/FileManager.h"

class SourceGroupCxx: public SourceGroup
{
public:
	SourceGroupCxx(std::shared_ptr<SourceGroupSettingsCxx> settings);
	virtual ~SourceGroupCxx();

	virtual SourceGroupType getType() const;

	virtual bool prepareRefresh();

	virtual void fetchAllSourceFilePaths();

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const bool fullRefresh);

private:
	std::shared_ptr<SourceGroupSettingsCxx> m_settings;
};

#endif // SOURCE_GROUP_CXX_H
