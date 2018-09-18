#ifndef SOURCE_GROUP_FACTORY_MODULE_H
#define SOURCE_GROUP_FACTORY_MODULE_H

#include <memory>

#include "SourceGroupType.h"

class SourceGroup;
class SourceGroupSettings;

class SourceGroupFactoryModule
{
public:
	virtual ~SourceGroupFactoryModule();
	virtual bool supports(SourceGroupType type) const = 0;
	virtual std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings) const = 0;
};

#endif // SOURCE_GROUP_FACTORY_MODULE_H
