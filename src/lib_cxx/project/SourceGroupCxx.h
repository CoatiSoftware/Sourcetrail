#ifndef SOURCE_GROUP_CXX_H
#define SOURCE_GROUP_CXX_H

#include <memory>
#include <set>

#include "project/SourceGroup.h"
#include "settings/SourceGroupSettingsCxx.h"

class SourceGroupCxx: public SourceGroup
{
public:
	SourceGroupCxx();
	virtual ~SourceGroupCxx();

private:
	virtual std::shared_ptr<SourceGroupSettingsCxx> getSourceGroupSettingsCxx() = 0;
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings();
};

#endif // SOURCE_GROUP_CXX_H
