#ifndef SOURCE_GROUP_FACTORY_MODULE_JAVA_H
#define SOURCE_GROUP_FACTORY_MODULE_JAVA_H

#include "project/SourceGroupFactoryModule.h"

class SourceGroupFactoryModuleJava: public SourceGroupFactoryModule
{
public:
	virtual ~SourceGroupFactoryModuleJava();
	virtual bool supports(SourceGroupType type) const;
	virtual std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings);
};

#endif // SOURCE_GROUP_FACTORY_MODULE_JAVA_H
