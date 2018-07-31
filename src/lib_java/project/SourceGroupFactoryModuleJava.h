#ifndef SOURCE_GROUP_FACTORY_MODULE_JAVA_H
#define SOURCE_GROUP_FACTORY_MODULE_JAVA_H

#include "project/SourceGroupFactoryModule.h"

class SourceGroupFactoryModuleJava: public SourceGroupFactoryModule
{
public:
	bool supports(SourceGroupType type) const override;
	std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings) const override;
};

#endif // SOURCE_GROUP_FACTORY_MODULE_JAVA_H
