#ifndef CXX_PROJECT_H
#define CXX_PROJECT_H

#include <memory>

#include "settings/CxxProjectSettings.h"
#include "Project.h"

class CxxProject: public Project
{
public:
	virtual ~CxxProject();

protected:
	virtual std::shared_ptr<ProjectSettings> getProjectSettings();
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const;

private:
	CxxProject(
		std::shared_ptr<CxxProjectSettings> projectSettings,
		StorageAccessProxy* storageAccessProxy,
		DialogView* dialogView
	);
	CxxProject(const CxxProject&);

	virtual bool allowsRefresh();

	virtual std::shared_ptr<Task> createIndexerTask(
		PersistentStorage* storage,
		std::shared_ptr<std::mutex> storageMutex,
		std::shared_ptr<FileRegister> fileRegister);

	virtual void updateFileManager(FileManager& fileManager);

	Parser::Arguments getParserArguments() const;

	std::shared_ptr<CxxProjectSettings> m_projectSettings;


	friend Project;
};

#endif // CXX_PROJECT_H
