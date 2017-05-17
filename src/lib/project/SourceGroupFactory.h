#ifndef SOURCE_GROUP_FACTORY_H
#define SOURCE_GROUP_FACTORY_H

#include <memory>
#include <vector>

class SourceGroup;
class SourceGroupSettings;
class SourceGroupFactoryModule;

class SourceGroupFactory
{
public:
	static std::shared_ptr<SourceGroupFactory> getInstance();

	void addModule(std::shared_ptr<SourceGroupFactoryModule> module);

	std::vector<std::shared_ptr<SourceGroup>> createSourceGroups(std::vector<std::shared_ptr<SourceGroupSettings>> allSourceGroupSettings);
	std::shared_ptr<SourceGroup> createSourceGroup(std::shared_ptr<SourceGroupSettings> settings);

private:
	static std::shared_ptr<SourceGroupFactory> s_instance;
	SourceGroupFactory();

	std::vector<std::shared_ptr<SourceGroupFactoryModule>> m_modules;
};

#endif // SOURCE_GROUP_FACTORY_H
