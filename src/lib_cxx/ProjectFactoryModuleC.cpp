#include "ProjectFactoryModuleC.h"

#include "settings/LanguageType.h"
#include "CxxProject.h"

ProjectFactoryModuleC::~ProjectFactoryModuleC()
{
}

LanguageType ProjectFactoryModuleC::getLanguage() const
{
	return LANGUAGE_C;
}

std::shared_ptr<Project> ProjectFactoryModuleC::createProject(
	const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, std::shared_ptr<DialogView> dialogView
)
{
	return std::shared_ptr<CxxProject>(new CxxProject(
		std::make_shared<CxxProjectSettings>(projectSettingsFile), storageAccessProxy, dialogView));
}
