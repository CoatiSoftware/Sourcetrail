#include "ProjectFactory.h"

#include "component/view/DialogView.h"
#include "data/access/StorageAccessProxy.h"
#include "settings/ProjectSettings.h"
#include "utility/file/FilePath.h"

#include "Project.h"
#include "ProjectFactoryModule.h"

void ProjectFactory::addModule(std::shared_ptr<ProjectFactoryModule> module)
{
	m_modules[module->getLanguage()] = module;
}

std::shared_ptr<Project> ProjectFactory::createProject(
	const FilePath& projectSettingsFile, StorageAccessProxy* storageAccessProxy, std::shared_ptr<DialogView> dialogView)
{
	std::shared_ptr<Project> project;

	std::map<LanguageType, std::shared_ptr<ProjectFactoryModule>>::const_iterator it = m_modules.find(
		ProjectSettings::getLanguageOfProject(projectSettingsFile)
	);

	if (it != m_modules.end())
	{
		project = it->second->createProject(projectSettingsFile, storageAccessProxy, dialogView);
	}

	if (project)
	{
		project->load();
	}
	return project;
}
