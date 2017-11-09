#include "project/SourceGroupJava.h"

#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaParser.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/ScopedFunctor.h"
#include "utility/utilityJava.h"
#include "Application.h"

SourceGroupJava::SourceGroupJava()
{
}

SourceGroupJava::~SourceGroupJava()
{
}

bool SourceGroupJava::prepareIndexing()
{
	if (!prepareJavaEnvironment())
	{
		return false;
	}
	return true;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(
	const std::set<FilePath>& filesToIndex, bool fullRefresh)
{
	const std::string languageStandard = getSourceGroupSettingsJava()->getStandard();

	std::vector<FilePath> classPath = getClassPath();
	std::set<FilePath> indexedPaths = getIndexedPaths();
	std::set<FilePath> excludedPaths = getExcludedPaths();

	const std::set<FilePath>& sourceFilePathsToIndex = (fullRefresh ? getAllSourceFilePaths() : getSourceFilePathsToIndex());

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: sourceFilePathsToIndex)
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			indexerCommands.push_back(
				std::make_shared<IndexerCommandJava>(sourcePath, indexedPaths, excludedPaths, languageStandard, classPath));
		}
	}

	return indexerCommands;
}

std::shared_ptr<SourceGroupSettings> SourceGroupJava::getSourceGroupSettings()
{
	return getSourceGroupSettingsJava();
}

bool SourceGroupJava::prepareJavaEnvironment()
{
	const std::string errorString = utility::prepareJavaEnvironment();

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


std::vector<FilePath> SourceGroupJava::getClassPath()
{
	LOG_INFO("Retrieving classpath for indexer commands");
	std::vector<FilePath> classPath = doGetClassPath();
	LOG_INFO("Found " + std::to_string(classPath.size()) + " paths for classpath.");
	return classPath;
}

std::vector<FilePath> SourceGroupJava::doGetClassPath()
{
	std::vector<FilePath> classPath;

	for (const FilePath& classpath : getSourceGroupSettingsJava()->getClasspathExpandedAndAbsolute())
	{
		if (classpath.exists())
		{
			LOG_INFO("Adding path to classpath: " + classpath.str());
			classPath.push_back(classpath);
		}
	}

	if (getSourceGroupSettingsJava()->getUseJreSystemLibrary())
	{
		for (const FilePath& systemLibraryPath : ApplicationSettings::getInstance()->getJreSystemLibraryPathsExpanded())
		{
			LOG_INFO("Adding JRE system library path to classpath: " + systemLibraryPath.str());
			classPath.push_back(systemLibraryPath);
		}
	}

	for (const FilePath& rootDirectory : fetchRootDirectories())
	{
		if (rootDirectory.exists())
		{
			LOG_INFO("Adding root directory to classpath: " + rootDirectory.str());
			classPath.push_back(rootDirectory);
		}
	}

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
		javaEnvironment->callStaticStringMethod("com/sourcetrail/JavaIndexer", "getPackageName", packageName, textAccess->getText());

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
