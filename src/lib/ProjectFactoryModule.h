#ifndef PROJECT_FACTORY_MODULE_H
#define PROJECT_FACTORY_MODULE_H

#include <memory>

#include "settings/LanguageType.h"

class Project;
class FilePath;
class StorageAccessProxy;
class DialogView;

class ProjectFactoryModule
{
public:
	virtual ~ProjectFactoryModule();
	virtual LanguageType getLanguage() const = 0;

	virtual std::shared_ptr<Project> createProject(
		const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
	) = 0;
};

#endif // PROJECT_FACTORY_MODULE_H
