#ifndef PROJECT_FACTORY_H
#define PROJECT_FACTORY_H

#include <map>
#include <memory>

#include "settings/LanguageType.h"

class Project;
class FilePath;
class StorageAccessProxy;
class DialogView;
class ProjectFactoryModule;

class ProjectFactory
{
public:
	void addModule(std::shared_ptr<ProjectFactoryModule> module);

	std::shared_ptr<Project> createProject(
		const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView);

private:
	std::map<LanguageType, std::shared_ptr<ProjectFactoryModule>> m_modules;
};

#endif // PROJECT_FACTORY_H
