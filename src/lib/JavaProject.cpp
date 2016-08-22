#include "JavaProject.h"

#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/TaskParseJava.h"
#include "isTrial.h"

JavaProject::~JavaProject()
{
}

std::shared_ptr<ProjectSettings> JavaProject::getProjectSettings()
{
	return m_projectSettings;
}

const std::shared_ptr<ProjectSettings> JavaProject::getProjectSettings() const
{
	return m_projectSettings;
}

JavaProject::JavaProject(std::shared_ptr<JavaProjectSettings> projectSettings, StorageAccessProxy* storageAccessProxy)
	: Project(storageAccessProxy)
	, m_projectSettings(projectSettings)
{
	if (!JavaEnvironmentFactory::getInstance() && !isTrial())
	{
#ifdef _WIN32
		const std::string separator = ";";
#else
		const std::string separator = ":";
#endif
		JavaEnvironmentFactory::createInstance(
			"data/java/guava-18.0.jar" + separator +
			"data/java/java-indexer.jar" + separator +
			"data/java/javaparser-core.jar" + separator +
			"data/java/javaslang-2.0.0-beta.jar" + separator +
			"data/java/javassist-3.19.0-GA.jar" + separator +
			"data/java/java-symbol-solver-core.jar" + separator +
			"data/java/java-symbol-solver-logic.jar" + separator +
			"data/java/java-symbol-solver-model.jar" + separator
		);
	}
}

std::shared_ptr<Task> JavaProject::createIndexerTask(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister)
{
	Parser::Arguments arguments;

	for (FilePath classpath: m_projectSettings->getAbsoluteClasspaths())
	{
		if (classpath.exists())
		{
			arguments.javaClassPaths.push_back(classpath.str());
		}
	}

	for (FilePath sourcePath: m_projectSettings->getAbsoluteSourcePaths())
	{
		if (sourcePath.extension().empty() && sourcePath.exists())
		{
			arguments.javaClassPaths.push_back(sourcePath.str());
		}
	}

	return std::make_shared<TaskParseJava>(
		storage,
		storageMutex,
		fileRegister,
		arguments
	);
}

void JavaProject::updateFileManager(FileManager& fileManager)
{
	std::vector<FilePath> sourcePaths = m_projectSettings->getAbsoluteSourcePaths();
	std::vector<FilePath> headerPaths = sourcePaths;
	std::vector<std::string> sourceExtensions = m_projectSettings->getSourceExtensions();
	std::vector<FilePath> excludePaths = m_projectSettings->getAbsoluteExcludePaths();

	fileManager.setPaths(sourcePaths, headerPaths, excludePaths, sourceExtensions);
}
