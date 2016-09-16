#ifndef PROJECT_FACTORY_MODULE_CPP_H
#define PROJECT_FACTORY_MODULE_CPP_H

#include "ProjectFactoryModule.h"

class ProjectFactoryModuleCpp: public ProjectFactoryModule
{
public:
	virtual ~ProjectFactoryModuleCpp();
	virtual LanguageType getLanguage() const;

	virtual std::shared_ptr<Project> createProject(
		const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
	);
};

#endif // PROJECT_FACTORY_MODULE_CPP_H
