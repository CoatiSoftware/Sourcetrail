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
#include "utility/utilityString.h"
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

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupJava::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	const std::string languageStandard = getSourceGroupSettingsJava()->getStandard();

	std::vector<FilePath> classPath = getClassPath();
	std::set<FilePath> indexedPaths = getIndexedPaths();
	std::set<FilePath> excludedPaths = getExcludedPaths();

	std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
	for (const FilePath& sourcePath: getAllSourceFilePaths())
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

std::shared_ptr<const SourceGroupSettings> SourceGroupJava::getSourceGroupSettings() const
{
	return getSourceGroupSettingsJava();
}

bool SourceGroupJava::prepareJavaEnvironment()
{
	const std::wstring errorString = utility::decodeFromUtf8(utility::prepareJavaEnvironment());

	if (errorString.size() > 0)
	{
		LOG_ERROR(errorString);
		MessageStatus(errorString, true, false).dispatch();
	}

	if (!JavaEnvironmentFactory::getInstance())
	{
		std::wstring dialogMessage =
			L"Sourcetrail was unable to locate Java on this machine.\n"
			"Please make sure to provide the correct Java Path in the preferences.";

		if (!errorString.empty())
		{
			dialogMessage += L"\n\nError: " + errorString;
		}

		MessageStatus(dialogMessage, true, false).dispatch();

		Application::getInstance()->handleDialog(dialogMessage);
		return false;
	}
	return true;
}


std::vector<FilePath> SourceGroupJava::getClassPath() const
{
	LOG_INFO("Retrieving classpath for indexer commands");
	std::vector<FilePath> classPath = doGetClassPath();
	LOG_INFO("Found " + std::to_string(classPath.size()) + " paths for classpath.");
	return classPath;
}

std::vector<FilePath> SourceGroupJava::doGetClassPath() const
{
	std::vector<FilePath> classPath;

	for (const FilePath& classpath : getSourceGroupSettingsJava()->getClasspathExpandedAndAbsolute())
	{
		if (classpath.exists())
		{
			LOG_INFO(L"Adding path to classpath: " + classpath.wstr());
			classPath.push_back(classpath);
		}
	}

	if (getSourceGroupSettingsJava()->getUseJreSystemLibrary())
	{
		for (const FilePath& systemLibraryPath : ApplicationSettings::getInstance()->getJreSystemLibraryPathsExpanded())
		{
			LOG_INFO(L"Adding JRE system library path to classpath: " + systemLibraryPath.wstr());
			classPath.push_back(systemLibraryPath);
		}
	}

	for (const FilePath& rootDirectory : fetchRootDirectories())
	{
		if (rootDirectory.exists())
		{
			LOG_INFO(L"Adding root directory to classpath: " + rootDirectory.wstr());
			classPath.push_back(rootDirectory);
		}
	}

	return classPath;
}

std::set<FilePath> SourceGroupJava::fetchRootDirectories() const
{
	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
	dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gathering Root\nDirectories");

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

		FilePath rootPath = filePath.getParentDirectory();
		bool success = true;

		const std::vector<std::string> packageNameParts = utility::splitToVector(packageName, ".");
		for (std::vector<std::string>::const_reverse_iterator it = packageNameParts.rbegin(); it != packageNameParts.rend(); it++)
		{
			if (rootPath.fileName() != utility::decodeFromUtf8(*it))
			{
				success = false;
				break;
			}
			rootPath = rootPath.getParentDirectory();
		}

		if (success)
		{
			rootDirectories.insert(rootPath);
		}
	}

	return rootDirectories;
}
