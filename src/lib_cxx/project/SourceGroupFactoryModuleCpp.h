#ifndef SOURCE_GROUP_FACTORY_MODULE_CPP_H
#define SOURCE_GROUP_FACTORY_MODULE_CPP_H

#include "project/SourceGroupFactoryModule.h"

class SourceGroupFactoryModuleCpp: public SourceGroupFactoryModule
{
public:
	virtual ~SourceGroupFactoryModuleCpp();
	virtual bool supports(SourceGroupType type) const;
	virtual std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings);
};

#endif // SOURCE_GROUP_FACTORY_MODULE_CPP_H
