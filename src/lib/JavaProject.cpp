#include "JavaProject.h"

#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/TaskParseJava.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "Application.h"
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

JavaProject::JavaProject(
	std::shared_ptr<JavaProjectSettings> projectSettings, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
	: Project(storageAccessProxy, dialogView)
	, m_projectSettings(projectSettings)
{
}

bool JavaProject::prepareIndexing()
{
	std::string errorString;
	if (!JavaEnvironmentFactory::getInstance() && !isTrial())
	{
#ifdef _WIN32
		const std::string separator = ";";
#else
		const std::string separator = ":";
#endif
		JavaEnvironmentFactory::createInstance(
			ResourcePaths::getJavaPath() + "guava-18.0.jar" + separator +
			ResourcePaths::getJavaPath() + "java-indexer.jar" + separator +
			ResourcePaths::getJavaPath() + "javaparser-core.jar" + separator +
			ResourcePaths::getJavaPath() + "javaslang-2.0.0-beta.jar" + separator +
			ResourcePaths::getJavaPath() + "javassist-3.19.0-GA.jar" + separator +
			ResourcePaths::getJavaPath() + "java-symbol-solver-core.jar" + separator +
			ResourcePaths::getJavaPath() + "java-symbol-solver-logic.jar" + separator +
			ResourcePaths::getJavaPath() + "java-symbol-solver-model.jar",
			errorString
		);
	}

	if (errorString.size() > 0)
	{
		LOG_ERROR(errorString);
		MessageStatus(errorString, true, false).dispatch();
	}

	if (!JavaEnvironmentFactory::getInstance() && !isTrial())
	{
		std::string dialogMessage =
			"Coati was unable to locate Java on this machine.\nPlease make sure to provide the correct Java Path in the preferences.";

		if (errorString.size() > 0)
		{
			dialogMessage += "\n\nError: " + errorString;
		}

		Application::getInstance()->handleDialog(dialogMessage);
		return false;
	}

	return true;
}

std::shared_ptr<Task> JavaProject::createIndexerTask(
	std::shared_ptr<IntermediateStorage> storage,
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
		fileRegister,
		arguments,
		getDialogView()
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
