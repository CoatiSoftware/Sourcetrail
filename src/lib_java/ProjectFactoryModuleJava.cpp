#include "ProjectFactoryModuleJava.h"

#include "settings/LanguageType.h"
#include "JavaProject.h"

ProjectFactoryModuleJava::~ProjectFactoryModuleJava()
{
}

LanguageType ProjectFactoryModuleJava::getLanguage() const
{
	return LANGUAGE_JAVA;
}

std::shared_ptr<Project> ProjectFactoryModuleJava::createProject(
	const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
{
	return std::shared_ptr<JavaProject>(new JavaProject(
		std::make_shared<JavaProjectSettings>(projectSettingsFile), storageAccessProxy, dialogView));
}
