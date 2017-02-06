#include "JavaProject.h"

#include "component/view/DialogView.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/TaskParseJava.h"
#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"
#include "Application.h"

JavaProject::JavaProject(
	std::shared_ptr<JavaProjectSettings> projectSettings, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
	: Project(storageAccessProxy, dialogView)
	, m_projectSettings(projectSettings)
{
}

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

bool JavaProject::prepareIndexing()
{
	m_rootDirectories.reset();

	std::string errorString;
	if (!JavaEnvironmentFactory::getInstance())
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
			ResourcePaths::getJavaPath() + "javaslang-2.0.3.jar" + separator +
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

	if (!JavaEnvironmentFactory::getInstance())
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
	std::shared_ptr<StorageProvider> storageProvider,
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

	if (!m_rootDirectories)
	{
		getDialogView()->showStatusDialog("Preparing Project", "Gathering Root\nDirectories");
		fetchRootDirectories();
		getDialogView()->hideStatusDialog();
	}

	for (FilePath rootDirectory: *(m_rootDirectories.get()))
	{
		if (rootDirectory.exists())
		{
			arguments.javaClassPaths.push_back(rootDirectory.str());
		}
	}

	return std::make_shared<TaskParseJava>(
		storageProvider,
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

void JavaProject::fetchRootDirectories()
{
	m_rootDirectories = std::make_shared<std::set<FilePath>>();

	FileManager fileManager;
	fileManager.setPaths(
		m_projectSettings->getAbsoluteSourcePaths(),
		std::vector<FilePath>(),
		m_projectSettings->getAbsoluteExcludePaths(),
		m_projectSettings->getSourceExtensions()
	);

	FileManager::FileSets fileSets = fileManager.fetchFilePaths(std::vector<FileInfo>());
	std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();

	for (FilePath filePath: fileSets.addedFiles)
	{
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath.str());

		std::string packageName = "";
		javaEnvironment->callStaticMethod("io/coati/JavaIndexer", "getPackageName", packageName, textAccess->getText());

		if (packageName.empty())
		{
			continue;
		}

		FilePath rootPath = filePath.parentDirectory();
		bool success = true;

		const std::vector<std::string> packageNameParts = utility::splitToVector(packageName, ".");
		for (std::vector<std::string>::const_reverse_iterator it = packageNameParts.rbegin(); it != packageNameParts.rend(); it++)
		{
			if (rootPath.fileName() != (*it))
			{
				success = false;
				break;
			}
			rootPath = rootPath.parentDirectory();
		}

		if (success)
		{
			m_rootDirectories->insert(rootPath);
		}
	}
}
