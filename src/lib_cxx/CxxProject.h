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
		DialogView* dialogView
	);
	virtual ~CxxProject();

protected:
	virtual std::shared_ptr<ProjectSettings> getProjectSettings();
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const;

private:
	CxxProject(const CxxProject&);

	virtual bool prepareRefresh();

	virtual std::shared_ptr<Task> createIndexerTask(
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<FileRegister> fileRegister);

	virtual void updateFileManager(FileManager& fileManager);

	Parser::Arguments getParserArguments() const;

	std::shared_ptr<CxxProjectSettings> m_projectSettings;


	friend Project;
};

#endif // CXX_PROJECT_H
