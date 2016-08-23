#ifndef JAVA_PROJECT_H
#define JAVA_PROJECT_H

#include <memory>

#include "settings/JavaProjectSettings.h"
#include "Project.h"

class JavaProject: public Project
{
public:
	virtual ~JavaProject();

protected:
	virtual std::shared_ptr<ProjectSettings> getProjectSettings();
	virtual const std::shared_ptr<ProjectSettings> getProjectSettings() const;

private:
	JavaProject(
		std::shared_ptr<JavaProjectSettings> projectSettings,
		StorageAccessProxy* storageAccessProxy,
		DialogView* dialogView
	);
	JavaProject(const JavaProject&);

	virtual std::shared_ptr<Task> createIndexerTask(
		PersistentStorage* storage,
		std::shared_ptr<std::mutex> storageMutex,
		std::shared_ptr<FileRegister> fileRegister);

	virtual void updateFileManager(FileManager& fileManager);

	std::shared_ptr<JavaProjectSettings> m_projectSettings;

	friend Project;
};

#endif // JAVA_PROJECT_H
