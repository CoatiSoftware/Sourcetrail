#include "ProjectFactoryModuleCpp.h"

#include "settings/LanguageType.h"
#include "CxxProject.h"

ProjectFactoryModuleCpp::~ProjectFactoryModuleCpp()
{
}

LanguageType ProjectFactoryModuleCpp::getLanguage() const
{
	return LANGUAGE_CPP;
}

std::shared_ptr<Project> ProjectFactoryModuleCpp::createProject(
	const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
{
	return std::shared_ptr<CxxProject>(new CxxProject(
		std::make_shared<CxxProjectSettings>(projectSettingsFile), storageAccessProxy, dialogView));
}
