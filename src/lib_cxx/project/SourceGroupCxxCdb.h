#ifndef SOURCE_GROUP_CXX_CDB_H
#define SOURCE_GROUP_CXX_CDB_H

#include <memory>
#include <set>

#include "project/SourceGroupCxx.h"
#include "settings/SourceGroupSettingsCxxCdb.h"

class SourceGroupCxxCdb: public SourceGroupCxx
{
public:
	SourceGroupCxxCdb(std::shared_ptr<SourceGroupSettingsCxxCdb> settings);
	virtual ~SourceGroupCxxCdb();

	virtual SourceGroupType getType() const;

	virtual bool prepareIndexing();

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex);

private:
	virtual std::shared_ptr<SourceGroupSettingsCxx> getSourceGroupSettingsCxx();
	virtual std::vector<FilePath> getAllSourcePaths() const;

	std::shared_ptr<SourceGroupSettingsCxxCdb> m_settings;
};

#endif // SOURCE_GROUP_CXX_CDB_H
