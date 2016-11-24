#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentBuildFile.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "settings/CxxProjectSettings.h"
#include "settings/JavaProjectSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessagePluginPortChange.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollSpeedChange.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utilityString.h"

#include "Application.h"

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QtWindowStackElement(parent)
	, m_windowStack(this)
	, m_editing(false)
{
	connect(&m_windowStack, SIGNAL(push()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(pop()), this, SLOT(windowStackChanged()));

	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
	m_appSettings.setScrollSpeed(appSettings->getScrollSpeed());
	m_appSettings.setCoatiPort(appSettings->getCoatiPort());
	m_appSettings.setPluginPort(appSettings->getPluginPort());

	m_parserManager = std::make_shared<SolutionParserManager>();

	// wip
	// m_parserManager->pushSolutionParser(std::make_shared<SolutionParserCodeBlocks>());
}

void QtProjectWizzard::showWindow()
{
	QtWindowStackElement* element = m_windowStack.getTopWindow();

	if (element)
	{
		element->show();
	}
}

void QtProjectWizzard::hideWindow()
{
	QtWindowStackElement* element = m_windowStack.getTopWindow();

	if (element)
	{
		element->hide();
	}
}

void QtProjectWizzard::newProject()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSelect>();

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		SIGNAL(selected(LanguageType, QtProjectWizzardContentSelect::ProjectType)),
		this, SLOT(selectedProjectType(LanguageType, QtProjectWizzardContentSelect::ProjectType)));

	window->setNextEnabled(false);
	window->setPreviousEnabled(false);
}

void QtProjectWizzard::newProjectFromSolution(const std::string& ideId, const std::string& visualStudioSolutionPath)
{
	if (m_parserManager->canParseSolution(ideId))
	{
		std::shared_ptr<ProjectSettings> settings = m_parserManager->getProjectSettings(ideId, visualStudioSolutionPath);

		// this looks rather hacky.. calling the edit project and then setting the title.
		editProject(settings);

		QWidget* widget = m_windowStack.getTopWindow();

		if (widget)
		{
			QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(widget);

			std::string title = "NEW PROJECT FROM ";
			title += utility::toUpperCase(ideId);
			title += " SOLUTION";

			window->updateTitle(QString(title.c_str()));
			window->updateNextButton("Create");
			window->setPreviousVisible(m_windowStack.getWindowCount() > 0);
		}
	}
	else
	{
		LOG_ERROR_STREAM(<< "Unknown solution type");
		MessageStatus("Unable to parse given solution", true).dispatch();
	}
}

void QtProjectWizzard::newProjectFromCDB(const std::string& filePath, const std::vector<std::string>& headerPaths)
{
	FilePath fp(filePath);

	std::vector<FilePath> hp;
	for (size_t i = 0; i < headerPaths.size(); i++)
	{
		hp.push_back(FilePath(headerPaths[i]));
	}

	std::shared_ptr<CxxProjectSettings> settings = std::make_shared<CxxProjectSettings>();
	settings->setLanguage(LanguageType::LANGUAGE_CPP);
	settings->setProjectName(fp.withoutExtension().fileName());
	settings->setProjectFileLocation(fp.parentDirectory());
	settings->setCompilationDatabasePath(fp);
	settings->setSourcePaths(hp);
	m_settings = settings;
	emptyProjectCDBVS();
}

void QtProjectWizzard::refreshProjectFromSolution(const std::string& ideId, const std::string& visualStudioSolutionPath)
{
	if (m_parserManager->canParseSolution(ideId))
	{
		QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(m_windowStack.getTopWindow());
		if (window)
		{
			window->content()->save();
		}

		std::shared_ptr<ProjectSettings> settings = m_parserManager->getProjectSettings(ideId, visualStudioSolutionPath);

		std::shared_ptr<CxxProjectSettings> otherCxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(settings);
		std::shared_ptr<CxxProjectSettings> ownCxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
		if (otherCxxSettings && ownCxxSettings)
		{
			ownCxxSettings->setSourcePaths(otherCxxSettings->getSourcePaths());
			ownCxxSettings->setHeaderSearchPaths(otherCxxSettings->getHeaderSearchPaths());
			ownCxxSettings->setVisualStudioSolutionPath(FilePath(visualStudioSolutionPath));
		}
		if (window)
		{
			window->content()->load();
		}
	}
	else
	{
		LOG_ERROR_STREAM(<< "Unknown solution type");
		MessageStatus("Unable to parse given solution", true).dispatch();
	}
}

void QtProjectWizzard::editProject(const FilePath& settingsPath)
{
	std::shared_ptr<ProjectSettings> settings;
	switch (ProjectSettings::getLanguageOfProject(settingsPath))
	{
	case LANGUAGE_C:
	case LANGUAGE_CPP:
		settings = std::make_shared<CxxProjectSettings>(settingsPath);
		break;
	case LANGUAGE_JAVA:
		settings = std::make_shared<JavaProjectSettings>(settingsPath);
		break;
	default:
		return;
	}

	if (settings)
	{
		settings->reload();
		editProject(settings);
	}
}

void QtProjectWizzard::editProject(std::shared_ptr<ProjectSettings> settings)
{
	m_settings = settings;
	m_editing = true;

	switch (m_settings->getLanguage())
	{
		case LANGUAGE_JAVA:
			showSummaryJava();
			break;

		case LANGUAGE_C:
		case LANGUAGE_CPP:
			showSummary();
			break;

		default:
			break;
	}

	QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(m_windowStack.getTopWindow());
	if (!window)
	{
		return;
	}

	window->updateTitle("EDIT PROJECT");
	window->updateNextButton("Save");
	window->setPreviousVisible(false);
}

void QtProjectWizzard::showPreferences()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			summary->addContent(new QtProjectWizzardContentPreferences(m_settings, window), false, false);

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(m_settings, window), false, false);
			if (QSysInfo::macVersion() != QSysInfo::MV_None)
			{
				summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(m_settings, window), false, false);
			}

			window->setup();

			window->updateTitle("PREFERENCES");
			window->updateNextButton("Save");
			window->setPreviousVisible(false);
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(savePreferences()));
}

template<typename T>
QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent()
{
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	window->setContent(new T(m_settings, window));
	window->setPreferredSize(QSize(580, 340));
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

template<>
QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent<QtProjectWizzardContentSelect>()
{
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	QtProjectWizzardContentSelect* content = new QtProjectWizzardContentSelect(m_settings, window, m_parserManager);

	window->setContent(content);
	window->setPreferredSize(QSize(570, 380));
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithSummary(
	std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentSummary*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(m_settings, window);

	window->setContent(summary);
	window->setPreferredSize(QSize(750, 500));
	func(window, summary);

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizzard::cancelWizzard()
{
	m_windowStack.clearWindows();
	emit canceled();
}

void QtProjectWizzard::finishWizzard()
{
	m_windowStack.clearWindows();
	emit finished();
}

void QtProjectWizzard::windowStackChanged()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizzardWindow*>(window)->content()->load();
	}
}

void QtProjectWizzard::selectedProjectType(LanguageType languageType, QtProjectWizzardContentSelect::ProjectType type)
{
	switch (type)
	{
	case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		if (languageType == LANGUAGE_JAVA)
		{
			m_settings = std::make_shared<JavaProjectSettings>();
		}
		else
		{
			m_settings = std::make_shared<CxxProjectSettings>();
		}
		m_settings->setLanguage(languageType);
		emptyProject();
		break;

	case QtProjectWizzardContentSelect::PROJECT_MANAGED:
		/*{
			std::string fileEndings = "(";
			for (unsigned int i = 0; i < m_parserManager->getParserCount(); i++)
			{
				fileEndings += " *" + m_parserManager->getParserFileEnding(i);
			}
			fileEndings += ")";

			QString fileName = QFileDialog::getOpenFileName(
				this, tr("Open Solution"), "", fileEndings.c_str()
			);

			if (!fileName.isNull())
			{
				for (unsigned int i = 0; i < m_parserManager->getParserCount(); i++)
				{
					if (fileName.contains(m_parserManager->getParserFileEnding(i).c_str()))
					{
						newProjectFromSolution(m_parserManager->getParserIdeId(i), fileName.toStdString());
					}
				}
			}
			break;
		}*/

		m_settings = std::make_shared<CxxProjectSettings>();
		m_settings->setLanguage(languageType);
		emptyProjectCDBVS();
		break;

	case QtProjectWizzardContentSelect::PROJECT_CDB:
		m_settings = std::make_shared<CxxProjectSettings>();
		m_settings->setLanguage(languageType);
		emptyProjectCDB();
		break;
	}
}

void QtProjectWizzard::emptyProject()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentData>();

	if (m_settings->getLanguage() == LANGUAGE_JAVA)
	{
		connect(window, SIGNAL(next()), this, SLOT(sourcePathsJava()));
	}
	else
	{
		connect(window, SIGNAL(next()), this, SLOT(sourcePaths()));
	}
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(m_settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentExtensions(m_settings, window), false, true);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPaths()));
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentSimple(m_settings, window), false, true);
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(m_settings, window), false, true);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPathsDone()));
}

void QtProjectWizzard::headerSearchPathsDone()
{
	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		frameworkSearchPaths();
	}
	else
	{
		showSummary();
	}
}

void QtProjectWizzard::frameworkSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(m_settings, window), false, true);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
}

void QtProjectWizzard::emptyProjectCDBVS()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentDataCDBVS>();

	connect(window, SIGNAL(next()), this, SLOT(headerPathsCDB()));
}

void QtProjectWizzard::emptyProjectCDB()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentDataCDB>();

	connect(window, SIGNAL(next()), this, SLOT(headerPathsCDB()));
}

void QtProjectWizzard::headerPathsCDB()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentCDBSource(m_settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_settings, window), false, true);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
}

void QtProjectWizzard::sourcePathsJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSourceJava(m_settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsClassJava(m_settings, window), false, true);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummaryJava()));
}

void QtProjectWizzard::showSummary()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			QtProjectWizzardContentBuildFile* buildFile = new QtProjectWizzardContentBuildFile(m_settings, window);

			if (buildFile->getType() != QtProjectWizzardContentSelect::PROJECT_CDB)
			{
				summary->addContent(new QtProjectWizzardContentData(m_settings, window), false, false);

				if (buildFile->getType() == QtProjectWizzardContentSelect::PROJECT_MANAGED)
				{
					summary->addContent(buildFile, false, true);

					connect(dynamic_cast<QtProjectWizzardContentBuildFile*>(buildFile),
						SIGNAL(refreshVisualStudioSolution(const std::string&, const std::string&)),
						this, SLOT(refreshProjectFromSolution(const std::string&, const std::string&)));
				}

				summary->addContent(new QtProjectWizzardContentPathsSource(m_settings, window), false, true);
				summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_settings, window), false, true);
				summary->addContent(new QtProjectWizzardContentSimple(m_settings, window), false, false);
				summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(m_settings, window), false, false);

				if (QSysInfo::macVersion() != QSysInfo::MV_None)
				{
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_settings, window), false, true);
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(m_settings, window), false, false);
				}

				summary->addContent(new QtProjectWizzardContentExtensions(m_settings, window), true, false);
				summary->addContent(new QtProjectWizzardContentFlags(m_settings, window), true, true);
				summary->addContent(new QtProjectWizzardContentPathsExclude(m_settings, window), true, true);
			}
			else
			{
				summary->addContent(new QtProjectWizzardContentDataCDB(m_settings, window), false, false);
				summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_settings, window), false, true);

				summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_settings, window, true), false, true);
				summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(m_settings, window), false, false);

				if (QSysInfo::macVersion() != QSysInfo::MV_None)
				{
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_settings, window, true), false, true);
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(m_settings, window), false, false);
				}

				summary->addContent(new QtProjectWizzardContentFlags(m_settings, window), true, false);
				summary->addContent(new QtProjectWizzardContentPathsExclude(m_settings, window), true, true);
			}

			window->setup();

			window->updateTitle("NEW PROJECT - SUMMARY");
			window->updateNextButton("Create");
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::showSummaryJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			summary->addContent(new QtProjectWizzardContentData(m_settings, window), false, false);

			summary->addContent(new QtProjectWizzardContentPathsSourceJava(m_settings, window), false, true);
			summary->addContent(new QtProjectWizzardContentPathsClassJava(m_settings, window), false, true);

			summary->addContent(new QtProjectWizzardContentExtensions(m_settings, window), true, false);
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_settings, window), true, true);

			window->setup();

			window->updateTitle("NEW PROJECT - SUMMARY");
			window->updateNextButton("Create");
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::createProject()
{
	FilePath path = m_settings->getFilePath();

	m_settings->setVersion(ProjectSettings::VERSION);
	m_settings->save(path);

	bool forceRefreshProject = false;
	if (m_editing)
	{
		std::shared_ptr<Application> application = Application::getInstance();
		FilePath oldPath;

		Project* currentProject = application->getCurrentProject().get();
		if (currentProject)
		{
			oldPath = currentProject->getProjectSettingsFilePath();
		}

		if (oldPath.exists() && oldPath != path)
		{
			std::vector<std::string> options;
			options.push_back("Yes");
			options.push_back("No");
			int result = application->handleDialog(
				"You changed the project location. The project file (.coatiproject) and the database file (.coatidb) will "
				"be moved to the new location. Do you want to keep a copy of the files in the previous location?"
				, options
			);

			FilePath dbPath = FilePath(path).replaceExtension("coatidb");
			FilePath oldDbPath = FilePath(oldPath).replaceExtension("coatidb");

			if (result == 0)
			{
				FileSystem::copyFile(oldDbPath, dbPath);
			}
			else
			{
				FileSystem::remove(oldPath);
				FileSystem::rename(oldDbPath, dbPath);
			}
		}

		bool settingsChanged = false;

		if (application->getCurrentProject() != NULL)
		{
			settingsChanged = !(application->getCurrentProject()->settingsEqualExceptNameAndLocation(*(m_settings.get())));
		}

		bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

		if (settingsChanged || appSettingsChanged)
		{
			forceRefreshProject = true;
		}
	}

	MessageDispatchWhenLicenseValid(
		std::make_shared<MessageLoadProject>(path, forceRefreshProject)
	).dispatch();

	finishWizzard();
}

void QtProjectWizzard::savePreferences()
{
	bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

	if (m_appSettings.getScrollSpeed() != ApplicationSettings::getInstance()->getScrollSpeed())
	{
		MessageScrollSpeedChange(ApplicationSettings::getInstance()->getScrollSpeed()).dispatch();
	}

	if (m_appSettings.getCoatiPort() != ApplicationSettings::getInstance()->getCoatiPort() ||
		m_appSettings.getPluginPort() != ApplicationSettings::getInstance()->getPluginPort())
	{
		MessagePluginPortChange().dispatch();
	}

	if (appSettingsChanged)
	{
		Project* currentProject = Application::getInstance()->getCurrentProject().get();
		if (currentProject)
		{
			MessageDispatchWhenLicenseValid(
				std::make_shared<MessageLoadProject>(
					currentProject->getProjectSettingsFilePath(), true
				)
			).dispatch();
		}
	}
	else
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}

	cancelWizzard();
}
