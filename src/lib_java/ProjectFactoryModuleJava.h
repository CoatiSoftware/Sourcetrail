#ifndef PROJECT_FACTORY_MODULE_JAVA_H
#define PROJECT_FACTORY_MODULE_JAVA_H

#include "ProjectFactoryModule.h"

class ProjectFactoryModuleJava: public ProjectFactoryModule
{
public:
	virtual ~ProjectFactoryModuleJava();
	virtual LanguageType getLanguage() const;

	virtual std::shared_ptr<Project> createProject(
		const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, std::shared_ptr<DialogView> dialogView
	);
};

#endif // PROJECT_FACTORY_MODULE_JAVA_H
