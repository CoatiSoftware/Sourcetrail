#include "project/SourceGroupJava.h"

#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaEnvironment.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/ScopedFunctor.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityMaven.h"
#include "utility/utilityString.h"
#include "utility/utility.h"
#include "Application.h"


SourceGroupJava::SourceGroupJava(std::shared_ptr<SourceGroupSettingsJava> settings)
	: m_settings(settings)
{
}

SourceGroupJava::~SourceGroupJava()
{
}

SourceGroupType SourceGroupJava::getType() const
{
	return m_settings->getType();
}

bool SourceGroupJava::prepareIndexing()
{
	if (!prepareJavaEnvironment())
	{
		return false;
	}

	if (!prepareMavenData())
	{
		return false;
	}

	return true;
}

void SourceGroupJava::fetchAllSourceFilePaths()
{
	std::vector<FilePath> sourcePaths;
	if (m_settings->getAbsoluteMavenProjectFilePath().exists())
	{
		std::shared_ptr<Application> application = Application::getInstance();

		if (application && application->hasGUI())
		{
			application->getDialogView()->showStatusDialog("Preparing Project", "Maven\nFetching Source Directories");
		}

		const FilePath mavenPath(ApplicationSettings::getInstance()->getMavenPath());
		const FilePath projectRootPath = m_settings->getAbsoluteMavenProjectFilePath().parentDirectory();
		sourcePaths = utility::mavenGetAllDirectoriesFromEffectivePom(mavenPath, projectRootPath, m_settings->getShouldIndexMavenTests());

		if (application && application->hasGUI())
		{
			application->getDialogView()->hideStatusDialog();
		}
	}
	else
	{
		sourcePaths = m_settings->getAbsoluteSourcePaths();
	}

	m_sourceFilePathsToIndex.clear();
	FileManager fileManager;
	fileManager.update(sourcePaths, m_settings->getAbsoluteExcludePaths(), m_settings->getSourceExtensions());
	m_allSourceFilePaths = fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(const bool fullRefresh)
{
	std::vector<FilePath> classPath = getClassPath();

	std::set<FilePath> indexedPaths;
	for (FilePath p: m_settings->getAbsoluteSourcePaths())
	{
		if (p.exists())
		{
			indexedPaths.insert(p);
		}
	}

	std::set<FilePath> excludedPaths;
	for (FilePath p: m_settings->getAbsoluteExcludePaths())
	{
		if (p.exists())
		{
			excludedPaths.insert(p);
		}
	}

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());
	for (const FilePath& sourcePath: sourceFilePathsToIndex)
	{
		indexerCommands.push_back(std::make_shared<IndexerCommandJava>(sourcePath, indexedPaths, excludedPaths, classPath));
	}

	return indexerCommands;
}

bool SourceGroupJava::prepareJavaEnvironment()
{
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
			"Sourcetrail was unable to locate Java on this machine.\n"
			"Please make sure to provide the correct Java Path in the preferences.";

		if (errorString.size() > 0)
		{
			dialogMessage += "\n\nError: " + errorString;
		}

		MessageStatus(dialogMessage, true, false).dispatch();

		Application::getInstance()->handleDialog(dialogMessage);
		return false;
	}
	return true;
}

bool SourceGroupJava::prepareMavenData()
{
	if (m_settings && m_settings->getAbsoluteMavenProjectFilePath().exists())
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath projectRootPath = m_settings->getAbsoluteMavenProjectFilePath().parentDirectory();

		ScopedFunctor dialogHider;

		std::shared_ptr<Application> application = Application::getInstance();

		if (application && application->hasGUI())
		{
			// this makes sure to hide the dialog when leaving this method.
			dialogHider = ScopedFunctor([](){
				Application::getInstance()->getDialogView()->hideStatusDialog();
			});
			Application::getInstance()->getDialogView()->showStatusDialog("Preparing Project", "Maven\nGenerating Source Files");
		}

		bool success = utility::mavenGenerateSources(mavenPath, projectRootPath);

		if (!success)
		{
			if (application && application->hasGUI())
			{
				const std::string dialogMessage =
					"Sourcetrail was unable to locate Maven on this machine.\n"
					"Please make sure to provide the correct Maven Path in the preferences.";

				MessageStatus(dialogMessage, true, false).dispatch();

				Application::getInstance()->handleDialog(dialogMessage);
			}
			return false;
		}

		if (application && application->hasGUI())
		{
			Application::getInstance()->getDialogView()->showStatusDialog("Preparing Project", "Maven\nExporting Dependencies");
		}

		utility::mavenCopyDependencies(
			mavenPath, projectRootPath, m_settings->getAbsoluteMavenDependenciesDirectory()
		);
	}

	return true;
}


std::vector<FilePath> SourceGroupJava::getClassPath()
{
	std::vector<FilePath> classPath;

	for (const FilePath& p: m_settings->getAbsoluteClasspaths())
	{
		if (p.exists())
		{
			classPath.push_back(p);
		}
	}

	if (m_settings->getAbsoluteMavenDependenciesDirectory().exists())
	{
		const std::vector<std::string> dependencies = FileSystem::getFileNamesFromDirectory(
			m_settings->getAbsoluteMavenDependenciesDirectory().str(),
			utility::createVectorFromElements<std::string>(".jar")
		);
		for (const std::string& dependency: dependencies)
		{
			classPath.push_back(dependency);
		}
	}

	for (FilePath rootDirectory: fetchRootDirectories())
	{
		if (rootDirectory.exists())
		{
			classPath.push_back(rootDirectory.str());
		}
	}

	return classPath;
}

std::set<FilePath> SourceGroupJava::fetchRootDirectories()
{
	if (std::shared_ptr<Application> application = Application::getInstance())
	{
		if (application->hasGUI())
		{
			application->getDialogView()->showStatusDialog("Preparing Project", "Gathering Root\nDirectories");
		}
	}
	std::set<FilePath> rootDirectories;

	std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
	for (FilePath filePath: m_allSourceFilePaths)
	{
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath.str());

		std::string packageName = "";
		javaEnvironment->callStaticMethod("com/sourcetrail/JavaIndexer", "getPackageName", packageName, textAccess->getText());

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
			rootDirectories.insert(rootPath);
		}
	}

	if (Application::getInstance()->hasGUI())
	{
		Application::getInstance()->getDialogView()->hideStatusDialog();
	}

	return rootDirectories;
}
