#include "project/SourceGroupJava.h"

#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/text/TextAccess.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityMaven.h"
#include "utility/utilityString.h"

#include "Application.h"
#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaParser.h"
#include "settings/ApplicationSettings.h"

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
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
		dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nFetching Source Directories");

		const FilePath mavenPath(ApplicationSettings::getInstance()->getMavenPath());
		const FilePath projectRootPath = m_settings->getAbsoluteMavenProjectFilePath().parentDirectory();
		sourcePaths = utility::mavenGetAllDirectoriesFromEffectivePom(mavenPath, projectRootPath, m_settings->getShouldIndexMavenTests());

		dialogView->hideUnknownProgressDialog();
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
	if (m_settings && m_settings->getAbsoluteMavenProjectFilePath().exists())
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath projectRootPath = m_settings->getAbsoluteMavenProjectFilePath().parentDirectory();

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
			return false;
		}

		dialogView->showUnknownProgressDialog("Preparing Project", "Maven\nExporting Dependencies");

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
			m_settings->getAbsoluteMavenDependenciesDirectory(),
			utility::createVectorFromElements<std::string>(".jar")
		);
		for (const std::string& dependency: dependencies)
		{
			classPath.push_back(FilePath(dependency));
		}
	}

	for (FilePath rootDirectory: fetchRootDirectories())
	{
		if (rootDirectory.exists())
		{
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
	for (FilePath filePath: m_allSourceFilePaths)
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
