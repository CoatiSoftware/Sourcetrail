#include "CxxProject.h"

#include "data/parser/cxx/TaskParseCxx.h"
#include "settings/ApplicationSettings.h"

#include "utility/file/FileRegister.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

#include "Application.h"

CxxProject::~CxxProject()
{
}

std::shared_ptr<ProjectSettings> CxxProject::getProjectSettings()
{
	return m_projectSettings;
}

const std::shared_ptr<ProjectSettings> CxxProject::getProjectSettings() const
{
	return m_projectSettings;
}

CxxProject::CxxProject(
	std::shared_ptr<CxxProjectSettings> projectSettings, StorageAccessProxy* storageAccessProxy, DialogView* dialogView
)
	: Project(storageAccessProxy, dialogView)
	, m_projectSettings(projectSettings)
{
}

bool CxxProject::allowsRefresh()
{
	FilePath cdbPath = m_projectSettings->getCompilationDatabasePath();
	if (!cdbPath.empty() && !cdbPath.exists())
	{
		MessageStatus("Can't refresh project").dispatch();

		if (Application::getInstance()->hasGUI())
		{
			std::vector<std::string> options;
			options.push_back("Ok");
			Application::getInstance()->handleDialog(
				"Can't refresh. The compilation database of the project does not exist anymore: " + cdbPath.str(),
				options
			);
		}
		return false;
	}
	return true;
}

std::shared_ptr<Task> CxxProject::createIndexerTask(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister)
{
	return std::make_shared<TaskParseCxx>(
		storage,
		storageMutex,
		fileRegister,
		getParserArguments(),
		getDialogView()
	);
}

void CxxProject::updateFileManager(FileManager& fileManager)
{
	std::vector<FilePath> sourcePaths = m_projectSettings->getAbsoluteSourcePaths();
	std::vector<FilePath> headerPaths = sourcePaths;

	std::vector<std::string> sourceExtensions;

	if (m_projectSettings->getCompilationDatabasePath().exists())
	{
		sourcePaths = TaskParseCxx::getSourceFilesFromCDB(m_projectSettings->getCompilationDatabasePath());
	}
	else
	{
		sourceExtensions = m_projectSettings->getSourceExtensions();
	}

	std::vector<FilePath> excludePaths = m_projectSettings->getAbsoluteExcludePaths();

	fileManager.setPaths(sourcePaths, headerPaths, excludePaths, sourceExtensions);
}

Parser::Arguments CxxProject::getParserArguments() const
{
	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();

	Parser::Arguments args;

	utility::append(args.compilerFlags, m_projectSettings->getCompilerFlags());

	// MAYBE DO THIS IN PARSER! AND ONLY ADD THE PATH OF THE CURRENT FILE!!
	// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
	//utility::append(args.systemHeaderSearchPaths, m_fileManager.getSourcePaths());

	utility::append(args.systemHeaderSearchPaths, m_projectSettings->getAbsoluteHeaderSearchPaths());

	utility::append(args.systemHeaderSearchPaths, appSettings->getHeaderSearchPathsExpanded());

	// Add all subdirectories of the header search paths
	if (m_projectSettings->getUseSourcePathsForHeaderSearch())
	{
		std::vector<FilePath> headerSearchSubPaths;
		for (FilePath p : m_projectSettings->getSourcePaths())
		{
			utility::append(headerSearchSubPaths, FileSystem::getSubDirectories(p));
		}

		utility::append(args.systemHeaderSearchPaths, utility::unique(headerSearchSubPaths));
	}

	utility::append(args.frameworkSearchPaths, m_projectSettings->getAbsoluteFrameworkSearchPaths());
	utility::append(args.frameworkSearchPaths, appSettings->getFrameworkSearchPathsExpanded());

	args.language = m_projectSettings->getLanguage();
	args.languageStandard = m_projectSettings->getStandard();
	args.compilationDatabasePath = m_projectSettings->getCompilationDatabasePath();

	return args;
}
