#ifndef SOURCE_GROUP_FACTORY_MODULE_CUSTOM_H
#define SOURCE_GROUP_FACTORY_MODULE_CUSTOM_H

#include "SourceGroupFactoryModule.h"

class SourceGroupFactoryModuleCustom: public SourceGroupFactoryModule
{
public:
	bool supports(SourceGroupType type) const override;
	std::shared_ptr<SourceGroup> createSourceGroup(
		std::shared_ptr<SourceGroupSettings> settings) const override;
};

#endif	  // SOURCE_GROUP_FACTORY_MODULE_CUSTOM_H
