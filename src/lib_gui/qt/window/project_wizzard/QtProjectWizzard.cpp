#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QMessageBox>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/messaging/type/MessageLoadProject.h"

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

void QtProjectWizzard::editProject(const ProjectSettings& settings)
{
	m_settings = settings;

	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSummary>();
	window->updateTitle("EDIT PROJECT");
	window->updateDoneButton("Save");

	window->hidePrevious();
	connect(window, SIGNAL(next()), this, SLOT(createProject()));
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

void QtProjectWizzard::selectedProjectType(QtProjectWizzardContentSelect::ProjectType type)
{
	switch (type)
	{
	case QtProjectWizzardContentSelect::PROJECT_EMPTY:
		emptyProject();
		break;

	case QtProjectWizzardContentSelect::PROJECT_CDB:
	case QtProjectWizzardContentSelect::PROJECT_VS:
		QMessageBox msgBox;
		msgBox.setText("Project type not implemented yet!");
		msgBox.exec();
		break;
	}
}

void QtProjectWizzard::emptyProject()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentData>();
	// connect(window, SIGNAL(next()), this, SLOT(simpleSetup()));
	connect(window, SIGNAL(next()), this, SLOT(sourcePaths()));
}

void QtProjectWizzard::simpleSetup()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSimple>();
	connect(window, SIGNAL(next()), this, SLOT(sourcePaths()));
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsSource>();
	connect(window, SIGNAL(next()), this, SLOT(headerSearchPaths()));
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsHeaderSearch>();

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		connect(window, SIGNAL(next()), this, SLOT(frameworkSearchPaths()));
	}
	else
	{
		connect(window, SIGNAL(next()), this, SLOT(showSummary()));
	}
}

void QtProjectWizzard::frameworkSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentPathsFrameworkSearch>();
	connect(window, SIGNAL(next()), this, SLOT(showSummary()));
}

void QtProjectWizzard::showSummary()
{
	QtProjectWizzardWindow* window = createWindowWithContent<QtProjectWizzardContentSummary>();
	connect(window, SIGNAL(next()), this, SLOT(createProject()));
}

void QtProjectWizzard::createProject()
{
	std::string path = m_settings.getProjectFileLocation() + "/" + m_settings.getProjectName() + ".coatiproject";

	m_settings.save(path);

	MessageLoadProject(path, true).dispatch();

	m_windowStack.clearWindows();
	emit finished();
}
