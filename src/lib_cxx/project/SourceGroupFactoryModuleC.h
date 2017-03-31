#ifndef SOURCE_GROUP_FACTORY_MODULE_C_H
#define SOURCE_GROUP_FACTORY_MODULE_C_H

#include "project/SourceGroupFactoryModule.h"

class SourceGroupFactoryModuleC: public SourceGroupFactoryModule
{
public:
	virtual ~SourceGroupFactoryModuleC();
	virtual bool supports(SourceGroupType type) const;
	virtual std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings);
};

#endif // SOURCE_GROUP_FACTORY_MODULE_C_H
