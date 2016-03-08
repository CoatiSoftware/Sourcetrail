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
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceList.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/solution/SolutionParserVisualStudio.h"

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QtWindowStackElement(parent)
	, m_windowStack(this)
{
	connect(&m_windowStack, SIGNAL(push()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(pop()), this, SLOT(windowStackChanged()));
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
	m_settings = ProjectSettings();

	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSelect>();

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		SIGNAL(selected(QtProjectWizzardContentSelect::ProjectType)),
		this, SLOT(selectedProjectType(QtProjectWizzardContentSelect::ProjectType)));

	window->setNextEnabled(false);
	window->setPreviousEnabled(false);
}

void QtProjectWizzard::newProjectFromVisualStudioSolution(const std::string& visualStudioSolutionPath)
{
	ProjectSettings settings = getSettingsForVisualStudioSolution(visualStudioSolutionPath);

	editProject(settings);

	QWidget* widget = m_windowStack.getTopWindow();

	if (widget)
	{
		QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(widget);

		window->updateTitle("NEW PROJECT FROM VS SOLUTION");
		window->updateNextButton("Create");
		window->setPreviousVisible(m_windowStack.getWindowCount() > 0);
	}
}

void QtProjectWizzard::refreshProjectFromVisualStudioSolution(const std::string& visualStudioSolutionPath)
{
	QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(m_windowStack.getTopWindow());
	if (window)
	{
		window->content()->save();
	}

	ProjectSettings settings = getSettingsForVisualStudioSolution(visualStudioSolutionPath);

	m_settings.setSourcePaths(settings.getSourcePaths());
	m_settings.setHeaderSearchPaths(settings.getHeaderSearchPaths());
	m_settings.setVisualStudioSolutionPath(FilePath(visualStudioSolutionPath));

	if (window)
	{
		window->content()->load();
	}
}

void QtProjectWizzard::newProjectFromCompilationDatabase(const std::string& compilationDatabasePath)
{
	m_settings = getSettingsForCompilationDatabase(compilationDatabasePath);

	headerPathsCDB();
}

void QtProjectWizzard::refreshProjectFromCompilationDatabase(const std::string& compilationDatabasePath)
{
	QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(m_windowStack.getTopWindow());
	if (window)
	{
		window->content()->save();
	}

	ProjectSettings settings = getSettingsForCompilationDatabase(compilationDatabasePath);

	m_settings.setSourcePaths(settings.getSourcePaths());
	m_settings.setHeaderSearchPaths(settings.getHeaderSearchPaths());
	m_settings.setFrameworkSearchPaths(settings.getFrameworkSearchPaths());
	m_settings.setCompilationDatabasePath(FilePath(compilationDatabasePath));

	if (window)
	{
		window->content()->load();
	}
}

void QtProjectWizzard::editProject(const ProjectSettings& settings)
{
	m_settings = settings;

	showSummary();

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

			ProjectSettings* settings = &m_settings;

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window), false, false);

			if (QSysInfo::macVersion() != QSysInfo::MV_None)
			{
				summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window), false, false);
			}

			window->setup();

			window->updateTitle("PREFERENCES");
			window->updateNextButton("Save");
			window->setPreviousVisible(false);
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(cancelWizzard()));
}

template<typename T>
QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent()
{
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

	window->setContent(new T(&m_settings, window));
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

	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(&m_settings, window);

	window->setContent(summary);
	func(window, summary);

	m_windowStack.pushWindow(window);

	return window;
}

template<typename T>
QtProjectWizzardWindow* QtProjectWizzard::createPopupWithContent()
{
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	window->setShowAsPopup(true);
	window->setContent(new T(&m_settings, window));
	window->setup();

	window->move(window->pos() + QPoint(50, 50));
	window->show();

	connect(window, SIGNAL(next()), this, SLOT(popupClosed()));

	if (m_popup)
	{
		m_popup->hide();
	}

	m_popup = std::shared_ptr<QtProjectWizzardWindow>(window);

	return window;
}

ProjectSettings QtProjectWizzard::getSettingsForVisualStudioSolution(const std::string& visualStudioSolutionPath) const
{
	SolutionParserVisualStudio parser;
	parser.openSolutionFile(visualStudioSolutionPath);

	ProjectSettings settings;
	settings.setProjectName(parser.getSolutionName());
	settings.setProjectFileLocation(parser.getSolutionPath());
	settings.setVisualStudioSolutionPath(FilePath(visualStudioSolutionPath));

	std::vector<std::string> sourceFiles = parser.getProjectItems();
	std::vector<FilePath> sourcePaths;
	for (const std::string& p : sourceFiles)
	{
		sourcePaths.push_back(FilePath(p));
	}

	std::vector<std::string> includePaths = parser.getIncludePaths();
	std::vector<FilePath> headerPaths;
	for (const std::string& p : includePaths)
	{
		headerPaths.push_back(FilePath(p));
	}

	settings.setSourcePaths(sourcePaths);
	settings.setHeaderSearchPaths(headerPaths);

	// For testing

	// ProjectSettings settings;
	// settings.setProjectName("hallo");
	// settings.setProjectFileLocation("~/Desktop");
	// settings.setVisualStudioSolutionPath(FilePath(visualStudioSolutionPath));
	// settings.setSourcePaths(std::vector<FilePath>(1, visualStudioSolutionPath));

	return settings;
}

ProjectSettings QtProjectWizzard::getSettingsForCompilationDatabase(const std::string& compilationDatabasePath) const
{
	ProjectSettings settings;
	settings.setCompilationDatabasePath(FilePath(compilationDatabasePath));
	return settings;
}

void QtProjectWizzard::connectShowFiles(QtProjectWizzardContent* content)
{
	connect(content, SIGNAL(filesButtonClicked(QtProjectWizzardContent*)),
		this, SLOT(showFiles(QtProjectWizzardContent*)));
}

void QtProjectWizzard::cancelWizzard()
{
	m_windowStack.clearWindows();
	emit canceled();
}

void QtProjectWizzard::windowStackChanged()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizzardWindow*>(window)->content()->load();
	}
}

void QtProjectWizzard::popupClosed()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		window->raise();
	}
}

void QtProjectWizzard::selectedProjectType(QtProjectWizzardContentSelect::ProjectType type)
{
	switch (type)
	{
	case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		emptyProject();
		break;

	case QtProjectWizzardContentSelect::PROJECT_VS:
		{
			QString fileName = QFileDialog::getOpenFileName(
				this, tr("Open Visual Studio Solution"), "", "Visual Studio Solution (*.sln)"
			);

			if (!fileName.isNull())
			{
				newProjectFromVisualStudioSolution(fileName.toStdString());
			}
			break;
		}

	case QtProjectWizzardContentSelect::PROJECT_CDB:
		{
			QString fileName = QFileDialog::getOpenFileName(
				this, tr("Open JSON Compilation Database"), "", "JSON Compilation Database (*.json)"
			);

			if (!fileName.isNull())
			{
				newProjectFromCompilationDatabase(fileName.toStdString());
			}
			break;
		}
		break;
	}
}

void QtProjectWizzard::emptyProject()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentData>();
	connect(window, SIGNAL(next()), this, SLOT(simpleSetup()));
}

void QtProjectWizzard::simpleSetup()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSimple>();
	connect(window, SIGNAL(next()), this, SLOT(simpleSetupDone()));
}

void QtProjectWizzard::simpleSetupDone()
{
	if (m_settings.getUseSourcePathsForHeaderSearch())
	{
		simpleSourcePaths();
	}
	else
	{
		sourcePaths();
	}
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsSource>();
	connect(window, SIGNAL(next()), this, SLOT(headerSearchPaths()));
	connectShowFiles(window->content());
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			ProjectSettings* settings = &m_settings;

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window), false, false);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPathsDone()));
}

void QtProjectWizzard::simpleSourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsSourceSimple>();
	connect(window, SIGNAL(next()), this, SLOT(simpleHeaderSearchPaths()));
	connectShowFiles(window->content());
}

void QtProjectWizzard::simpleHeaderSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			ProjectSettings* settings = &m_settings;

			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchSimple(settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window), false, false);

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
			ProjectSettings* settings = &m_settings;

			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(settings, window), false, false);
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window), false, false);

			window->setup();
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
}

void QtProjectWizzard::headerPathsCDB()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			ProjectSettings* settings = &m_settings;

			QtProjectWizzardContent* source = new QtProjectWizzardContentCDBSource(settings, window);
			summary->addContent(source, false, false);
			connectShowFiles(source);

			QtProjectWizzardContent* header = new QtProjectWizzardContentPathsCDBHeader(settings, window);
			summary->addContent(header, false, false);
			connectShowFiles(header);

			window->setup();

			window->updateTitle("NEW PROJECT FROM COMPILATION DATABASE");
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerPathsCDBDone()));
}

void QtProjectWizzard::headerPathsCDBDone()
{
	showSummary();

	QWidget* widget = m_windowStack.getTopWindow();

	if (widget)
	{
		QtProjectWizzardWindow* window = dynamic_cast<QtProjectWizzardWindow*>(widget);

		window->updateTitle("NEW PROJECT FROM COMPILATION DATABASE - SUMMARY");
		window->updateNextButton("Create");
		window->setPreviousVisible(m_windowStack.getWindowCount() > 0);
	}
}

void QtProjectWizzard::showFiles(QtProjectWizzardContent* content)
{
	QtProjectWizzardWindow* window = createPopupWithContent<QtProjectWizzardContentSourceList>();
	dynamic_cast<QtProjectWizzardContentSourceList*>(window->content())->showFilesFromContent(content);
}

void QtProjectWizzard::showSummary()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->setIsForm(true);

			ProjectSettings* settings = &m_settings;

			QtProjectWizzardContentData* data = new QtProjectWizzardContentData(settings, window);
			summary->addContent(data, false, false);

			QtProjectWizzardContentBuildFile* buildFile = new QtProjectWizzardContentBuildFile(settings, window);

			if (buildFile->getType() != QtProjectWizzardContentSelect::PROJECT_EMPTY)
			{
				summary->addContent(buildFile, false, true);

				connect(dynamic_cast<QtProjectWizzardContentBuildFile*>(buildFile),
					SIGNAL(refreshVisualStudioSolution(const std::string&)),
					this, SLOT(refreshProjectFromVisualStudioSolution(const std::string&)));
			}

			if (buildFile->getType() != QtProjectWizzardContentSelect::PROJECT_CDB)
			{
				QtProjectWizzardContentPathsSource* source = new QtProjectWizzardContentPathsSource(settings, window);
				summary->addContent(source, false, true);
				connectShowFiles(source);

				summary->addContent(new QtProjectWizzardContentSimple(settings, window), false, true);
				summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(settings, window), false, false);
				summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window), false, false);

				if (QSysInfo::macVersion() != QSysInfo::MV_None)
				{
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(settings, window), false, true);
					summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window), false, false);
				}
			}
			else
			{
				QtProjectWizzardContent* headers = new QtProjectWizzardContentPathsCDBHeader(settings, window);
				summary->addContent(headers, false, true);
				connectShowFiles(headers);

				data->hideLanguage();
			}

			summary->addContent(new QtProjectWizzardContentFlags(settings, window), true, false);
			summary->addContent(new QtProjectWizzardContentExtensions(settings, window), true, true);

			window->setup();

			window->updateTitle("NEW PROJECT - SUMMARY");
			window->updateNextButton("Create");
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::createProject()
{
	std::string path = m_settings.getProjectFileLocation() + "/" + m_settings.getProjectName() + ".coatiproject";

	m_settings.save(path);

	bool forceRefresh = !(m_settings == *ProjectSettings::getInstance().get());

	MessageLoadProject(path, forceRefresh).dispatch();

	m_windowStack.clearWindows();
	emit finished();
}
