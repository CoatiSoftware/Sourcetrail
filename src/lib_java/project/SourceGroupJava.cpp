#include "project/SourceGroupJava.h"

#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaParser.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityMaven.h"
#include "utility/utilityString.h"
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
	if (m_settings->getMavenProjectFilePathExpandedAndAbsolute().exists())
	{
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
		dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nFetching Source Directories");

		const FilePath mavenPath(ApplicationSettings::getInstance()->getMavenPath());
		const FilePath projectRootPath = m_settings->getMavenProjectFilePathExpandedAndAbsolute().parentDirectory();
		sourcePaths = utility::mavenGetAllDirectoriesFromEffectivePom(mavenPath, projectRootPath, m_settings->getShouldIndexMavenTests());

		dialogView->hideUnknownProgressDialog();
	}
	else
	{
		sourcePaths = m_settings->getSourcePathsExpandedAndAbsolute();
	}

	m_sourceFilePathsToIndex.clear();
	FileManager fileManager;
	fileManager.update(sourcePaths, m_settings->getExcludePathsExpandedAndAbsolute(), m_settings->getSourceExtensions());
	m_allSourceFilePaths = fileManager.getAllSourceFilePaths();
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(
	std::set<FilePath>* filesToIndex, bool fullRefresh)
{
	std::vector<FilePath> classPath = getClassPath();

	std::set<FilePath> indexedPaths;
	for (const FilePath& p: m_settings->getSourcePathsExpandedAndAbsolute())
	{
		if (p.exists())
		{
			indexedPaths.insert(p);
		}
	}

	std::set<FilePath> excludedPaths;
	for (const FilePath& p: m_settings->getExcludePathsExpandedAndAbsolute())
	{
		if (p.exists())
		{
			excludedPaths.insert(p);
		}
	}

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: sourceFilePathsToIndex)
	{
		if (filesToIndex->find(sourcePath) != filesToIndex->end())
		{
			indexerCommands.push_back(
				std::make_shared<IndexerCommandJava>(sourcePath, indexedPaths, excludedPaths, classPath));

			filesToIndex->erase(sourcePath);
		}
	}

	return indexerCommands;
}

bool SourceGroupJava::prepareJavaEnvironment()
{
	const std::string errorString = JavaParser::prepareJavaEnvironment();

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
	if (m_settings && m_settings->getMavenProjectFilePathExpandedAndAbsolute().exists())
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath projectRootPath = m_settings->getMavenProjectFilePathExpandedAndAbsolute().parentDirectory();

		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
		dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nGenerating Source Files");

		ScopedFunctor dialogHider([&dialogView](){
			dialogView->hideUnknownProgressDialog();
		});

		bool success = utility::mavenGenerateSources(mavenPath, projectRootPath);

		if (!success)
		{
			const std::string dialogMessage =
				"Sourcetrail was unable to locate Maven on this machine.\n"
				"Please make sure to provide the correct Maven Path in the preferences.";

			MessageStatus(dialogMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(dialogMessage);
		}

		if (success)
		{
			dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nExporting Dependencies");

			success = utility::mavenCopyDependencies(
				mavenPath, projectRootPath, m_settings->getMavenDependenciesDirectoryExpandedAndAbsolute()
			);
		}

		return success;
	}

	return true;
}


std::vector<FilePath> SourceGroupJava::getClassPath()
{
	LOG_INFO("Retrieving classpath for indexer commands");

	std::vector<FilePath> classPath;

	for (const FilePath& classpath: m_settings->getClasspathExpandedAndAbsolute())
	{
		if (classpath.exists())
		{
			LOG_INFO("Adding path to classpath: " + classpath.str());
			classPath.push_back(classpath);
		}
	}

	if (m_settings->getUseJreSystemLibrary())
	{
		for (const FilePath& systemLibraryPath: ApplicationSettings::getInstance()->getJreSystemLibraryPathsExpanded())
		{
			LOG_INFO("Adding JRE system library path to classpath: " + systemLibraryPath.str());
			classPath.push_back(systemLibraryPath);
		}
	}

	if (m_settings->getMavenDependenciesDirectoryExpandedAndAbsolute().exists())
	{
		std::vector<FilePath> mavenJarPaths = FileSystem::getFilePathsFromDirectory(
			m_settings->getMavenDependenciesDirectoryExpandedAndAbsolute(),
			utility::createVectorFromElements<std::string>(".jar")
		);

		for (const FilePath& mavenJarPath: mavenJarPaths)
		{
			LOG_INFO("Adding jar to classpath: " + mavenJarPath.str());
		}

		utility::append(classPath, mavenJarPaths);
	}

	for (const FilePath& rootDirectory: fetchRootDirectories())
	{
		if (rootDirectory.exists())
		{
			LOG_INFO("Adding root directory to classpath: " + rootDirectory.str());
			classPath.push_back(rootDirectory);
		}
	}
	LOG_INFO("Found " + std::to_string(classPath.size()) + " paths for classpath.");

	return classPath;
}

std::set<FilePath> SourceGroupJava::fetchRootDirectories()
{
	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
	dialogView->showUnknownProgressDialog("Preparing Project", "Gathering Root\nDirectories");

	ScopedFunctor dialogHider([&dialogView](){
		dialogView->hideUnknownProgressDialog();
	});

	std::set<FilePath> rootDirectories;

	std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
	for (const FilePath& filePath: m_allSourceFilePaths)
	{
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

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

	return rootDirectories;
}
