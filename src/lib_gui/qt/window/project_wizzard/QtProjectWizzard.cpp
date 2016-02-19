#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceList.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/solution/SolutionParserVisualStudio.h"

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QWidget(parent)
	, m_windowStack(this)
{
	connect(&m_windowStack, SIGNAL(push()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(pop()), this, SLOT(windowStackChanged()));
}

void QtProjectWizzard::newProject()
{
	m_settings = ProjectSettings();

	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSelect>();

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		SIGNAL(selected(QtProjectWizzardContentSelect::ProjectType)),
		this, SLOT(selectedProjectType(QtProjectWizzardContentSelect::ProjectType)));

	window->disableNext();
	window->disablePrevious();
}

void QtProjectWizzard::newProjectFromVisualStudioSolution(const std::string& visualStudioSolutionPath)
{
	ProjectSettings settings = getSettingsForVisualStudioSolution(visualStudioSolutionPath);

	editProject(settings);

	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizzardWindow*>(window)->updateTitle("NEW PROJECT FROM VS SOLUTION");
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
	window->updateDoneButton("Save");

	window->hidePrevious();
	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::showPreferences()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPreferences>();
	connect(window, SIGNAL(next()), this, SLOT(cancelWizzard()));
}

template<typename T>
QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent()
{
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	window->setContent(new T(&m_settings, window));
	window->setup();

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelWizzard()));

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

	connect(window, SIGNAL(closed()), this, SLOT(popupClosed()));

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
		QMessageBox msgBox;
		msgBox.setText("Project type not implemented yet!");
		msgBox.exec();
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

	connect(dynamic_cast<QtProjectWizzardContentPathsSource*>(window->content()),
		SIGNAL(showSourceFiles()), this, SLOT(showSourceFiles()));
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsHeaderSearch>();
	connect(window, SIGNAL(next()), this, SLOT(headerSearchPathsDone()));
}

void QtProjectWizzard::simpleSourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsSourceSimple>();
	connect(window, SIGNAL(next()), this, SLOT(simpleHeaderSearchPaths()));

	connect(dynamic_cast<QtProjectWizzardContentPathsSourceSimple*>(window->content()),
		SIGNAL(showSourceFiles()), this, SLOT(showSourceFiles()));
}

void QtProjectWizzard::simpleHeaderSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsHeaderSearchSimple>();
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
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsFrameworkSearch>();
	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
}

void QtProjectWizzard::showSourceFiles()
{
	QWidget* topWindow = m_windowStack.getTopWindow();
	if (topWindow)
	{
		dynamic_cast<QtProjectWizzardWindow*>(topWindow)->content()->save();
	}

	QtProjectWizzardWindow* window = createPopupWithContent<QtProjectWizzardContentSourceList>();
	dynamic_cast<QtProjectWizzardContentSourceList*>(window->content())->showFilesFromSourcePaths();
}

void QtProjectWizzard::showSummary()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSummary>();
	connect(window, SIGNAL(next()), this, SLOT(createProject()));

	QtProjectWizzardContentSummary* summary = dynamic_cast<QtProjectWizzardContentSummary*>(window->content());

	connect(dynamic_cast<QtProjectWizzardContentBuildFile*>(summary->contentBuildFile()),
		SIGNAL(refreshVisualStudioSolution(const std::string&)),
		this, SLOT(refreshProjectFromVisualStudioSolution(const std::string&)));

	connect(dynamic_cast<QtProjectWizzardContentPathsSource*>(summary->contentPathsSource()),
		SIGNAL(showSourceFiles()), this, SLOT(showSourceFiles()));
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
