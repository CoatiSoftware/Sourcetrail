#ifndef CXX_PROJECT_H
#define CXX_PROJECT_H

#include <memory>

#include "settings/CxxProjectSettings.h"
#include "Project.h"

class CxxProject: public Project
{
public:
	CxxProject(
		std::shared_ptr<CxxProjectSettings> projectSettings,
		StorageAccessProxy* storageAccessProxy,
		std::shared_ptr<DialogView> dialogView
	);
	virtual ~CxxProject();

protected:
	virtual std::shared_ptr<ProjectSettings> getProjectSettings();
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const;

private:
	CxxProject(const CxxProject&);

	virtual bool prepareRefresh();

	virtual std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands();

	virtual void updateFileManager(FileManager& fileManager);

	std::shared_ptr<CxxProjectSettings> m_projectSettings;


	friend Project;
};

#endif // CXX_PROJECT_H
