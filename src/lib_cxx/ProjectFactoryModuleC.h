#ifndef PROJECT_FACTORY_MODULE_C_H
#define PROJECT_FACTORY_MODULE_C_H

#include "ProjectFactoryModule.h"

class ProjectFactoryModuleC: public ProjectFactoryModule
{
public:
	virtual ~ProjectFactoryModuleC();
	virtual LanguageType getLanguage() const;

	virtual std::shared_ptr<Project> createProject(
		const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
	);
};

#endif // PROJECT_FACTORY_MODULE_C_H
