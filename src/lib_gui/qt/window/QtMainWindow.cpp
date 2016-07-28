#include "qt/window/QtMainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSysInfo>
#include <QTimer>

#include "component/view/View.h"
#include "component/view/CompositeView.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "qt/window/project_wizzard/QtProjectWizzard.h"
#include "qt/window/QtStartScreen.h"
#include "qt/window/QtAboutLicense.h"
#include "qt/window/QtAbout.h"
#include "qt/window/QtLicense.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageResetZoom.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "utility/messaging/type/MessageZoom.h"
#include "utility/ResourcePaths.h"
#include "utility/tracing.h"
#include "utility/UserPaths.h"
#include "version.h"
#include "isTrial.h"

QtViewToggle::QtViewToggle(View* view, QWidget *parent)
	: QWidget(parent)
	, m_view(view)
{
}

void QtViewToggle::toggledByAction()
{
	dynamic_cast<QtMainWindow*>(parent())->toggleView(m_view, true);
}

void QtViewToggle::toggledByUI()
{
	dynamic_cast<QtMainWindow*>(parent())->toggleView(m_view, false);
}


MouseReleaseFilter::MouseReleaseFilter(QObject* parent)
	: QObject(parent)
{
	m_backButton = ApplicationSettings::getInstance()->getControlsMouseBackButton();
	m_forwardButton = ApplicationSettings::getInstance()->getControlsMouseForwardButton();
}

bool MouseReleaseFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == m_backButton)
		{
			MessageUndo().dispatch();
			return true;
		}
		else if (mouseEvent->button() == m_forwardButton)
		{
			MessageRedo().dispatch();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

MouseWheelFilter::MouseWheelFilter(QObject* parent)
	: QObject(parent)
	, m_isWaiting(false)
{
}

bool MouseWheelFilter::eventFilter(QObject* obj, QEvent* event)
{
	bool dispatched = false;

	if (!m_isWaiting && event->type() == QEvent::Wheel && QApplication::keyboardModifiers() == Qt::ControlModifier)
	{
		QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);

		if (wheelEvent->delta() > 0.0f)
		{
			MessageZoom(true).dispatch();
			dispatched = true;
		}
		else if (wheelEvent->delta() < 0.0f)
		{
			MessageZoom(false).dispatch();
			dispatched = true;
		}
	}

	if (dispatched)
	{
		QTimer::singleShot(250, this, SLOT(stopWaiting()));
		m_isWaiting = true;
		return true;
	}

	return QObject::eventFilter(obj, event);
}

void MouseWheelFilter::stopWaiting()
{
	m_isWaiting = false;
}


QtMainWindow::QtMainWindow()
	: m_showDockWidgetTitleBars(true)
	, m_windowStack(this)
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setWindowIcon(QIcon((ResourcePaths::getGuiPath() + "icon/logo_1024_1024.png").c_str()));
	setWindowFlags(Qt::Widget);

	QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());
	app->installEventFilter(new MouseReleaseFilter(this));
	app->installEventFilter(new MouseWheelFilter(this));

	app->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "main.css").c_str());

	m_recentProjectAction = new QAction*[ApplicationSettings::getInstance()->getMaxRecentProjectsCount()];

	setupProjectMenu();
	setupEditMenu();
	setupViewMenu();
	setupHelpMenu();

	// Need to call loadLayout here for right DockWidget size on Linux
	// Seconde call is in Application.cpp
	loadLayout();
}

QtMainWindow::~QtMainWindow()
{
	if (m_recentProjectAction)
	{
		delete [] m_recentProjectAction;
	}
}

void QtMainWindow::addView(View* view)
{
	QDockWidget* dock = new QDockWidget(tr(view->getName().c_str()), this);
	dock->setWidget(QtViewWidgetWrapper::getWidgetOfView(view));
	dock->setObjectName(QString::fromStdString("Dock" + view->getName()));

	if (!m_showDockWidgetTitleBars)
	{
		dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		dock->setTitleBarWidget(new QWidget());
	}

	addDockWidget(Qt::TopDockWidgetArea, dock);

	QtViewToggle* toggle = new QtViewToggle(view, this);
	connect(dock, SIGNAL(visibilityChanged(bool)), toggle, SLOT(toggledByUI()));

	QAction* action = new QAction(tr((view->getName() + " Window").c_str()), this);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), toggle, SLOT(toggledByAction()));
	m_viewMenu->insertAction(m_viewSeparator, action);

	DockWidget dockWidget;
	dockWidget.widget = dock;
	dockWidget.view = view;
	dockWidget.action = action;
	dockWidget.toggle = toggle;

	m_dockWidgets.push_back(dockWidget);
}

void QtMainWindow::removeView(View* view)
{
	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		if (m_dockWidgets[i].view == view)
		{
			removeDockWidget(m_dockWidgets[i].widget);
			m_dockWidgets.erase(m_dockWidgets.begin() + i);
			return;
		}
	}
}

void QtMainWindow::showView(View* view)
{
	getDockWidgetForView(view)->widget->setHidden(false);
}

void QtMainWindow::hideView(View* view)
{
	getDockWidgetForView(view)->widget->setHidden(true);
}

void QtMainWindow::loadLayout()
{
	QSettings settings(UserPaths::getWindowSettingsPath().c_str(), QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(600, 400)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());
	if (settings.value("maximized", false).toBool())
	{
		showMaximized();
	}
	setShowDockWidgetTitleBars(settings.value("showTitleBars", true).toBool());
	settings.endGroup();

	this->restoreState(settings.value("DOCK_LOCATIONS").toByteArray());

	for (DockWidget dock : m_dockWidgets)
	{
		dock.action->setChecked(!dock.widget->isHidden());
	}
}

void QtMainWindow::saveLayout()
{
	QSettings settings(UserPaths::getWindowSettingsPath().c_str(), QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	settings.setValue("maximized", isMaximized());
	if (!isMaximized())
	{
		settings.setValue("size", size());
		settings.setValue("position", pos());
	}
	settings.setValue("showTitleBars", m_showDockWidgetTitleBars);
	settings.endGroup();

	settings.setValue("DOCK_LOCATIONS", this->saveState());
}

void QtMainWindow::forceEnterLicense(bool expired)
{
	enterLicense();

	QtLicense* enterLicenseWindow = dynamic_cast<QtLicense*>(m_windowStack.getTopWindow());
	if (!enterLicenseWindow)
	{
		LOG_ERROR("No enter license window on top of stack");
		return;
	}

	if (expired)
	{
		enterLicenseWindow->setErrorMessage("The license key is expired.");
	}
	else
	{
		enterLicenseWindow->clear();
	}

	enterLicenseWindow->loadForced();

	this->setEnabled(false);
	enterLicenseWindow->setEnabled(true);
}

bool QtMainWindow::event(QEvent* event)
{
	if (event->type() == QEvent::WindowActivate)
	{
		MessageWindowFocus().dispatch();
	}

	return QMainWindow::event(event);
}

void QtMainWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Backspace:
			MessageUndo().dispatch();
			break;

		case Qt::Key_Escape:
			MessageInterruptTasks().dispatch();
			break;

		case Qt::Key_Space:
			PRINT_TRACES();
			break;
	}
}

void QtMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu::getInstance()->showDefault(event, this);
}

void QtMainWindow::activateWindow()
{
	this->setEnabled(true);
	m_windowStack.popWindow();
}

void QtMainWindow::about()
{
	QtAbout* aboutWindow = createWindow<QtAbout>();
	aboutWindow->setupAbout();
}

void QtMainWindow::openSettings()
{
	QtProjectWizzard* wizzard = createWindow<QtProjectWizzard>();
	wizzard->showPreferences();
}

void QtMainWindow::showDocumentation()
{
	QDesktopServices::openUrl(QUrl("https://coati.io/documentation/"));
}

void QtMainWindow::showBugtracker()
{
	QDesktopServices::openUrl(QUrl("https://github.com/CoatiSoftware/CoatiBugTracker/issues"));
}

void QtMainWindow::showLicenses()
{
	QtAboutLicense* licenseWindow = createWindow<QtAboutLicense>();
	licenseWindow->setup();
}

void QtMainWindow::enterLicense()
{
	QtLicense* enterLicenseWindow = createWindow<QtLicense>();
	enterLicenseWindow->setup();

	disconnect(enterLicenseWindow, SIGNAL(finished()), &m_windowStack, SLOT(clearWindows()));
	connect(enterLicenseWindow, SIGNAL(finished()), this, SLOT(activateWindow()));

	enterLicenseWindow->load();
}

void QtMainWindow::showStartScreen()
{
	QtStartScreen* startScreen = createWindow<QtStartScreen>();
	startScreen->setupStartScreen();

	connect(startScreen, SIGNAL(openOpenProjectDialog()), this, SLOT(openProject()));
	connect(startScreen, SIGNAL(openNewProjectDialog()), this, SLOT(newProject()));
}

void QtMainWindow::hideStartScreen()
{
	m_windowStack.clearWindows();
}

void QtMainWindow::newProject()
{
	QtProjectWizzard* wizzard = createWindow<QtProjectWizzard>();
	wizzard->newProject();
}

void QtMainWindow::newProjectFromSolution(const std::string& ideId, const std::string& solutionPath)
{
	QtProjectWizzard* wizzard = createWindow<QtProjectWizzard>();
	wizzard->newProjectFromSolution(ideId, solutionPath);
}

void QtMainWindow::openProject(const QString &path)
{
	QString fileName = path;

	if (fileName.isNull())
	{
		fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Coati Project Files (*.coatiproject)");
	}

	if (!fileName.isEmpty())
	{
		MessageDispatchWhenLicenseValid(
			std::make_shared<MessageLoadProject>(fileName.toStdString(), false)
		).dispatch();
		m_windowStack.clearWindows();
	}
}

void QtMainWindow::editProject()
{
	QtProjectWizzard* wizzard = createWindow<QtProjectWizzard>();
	wizzard->editProject(*ProjectSettings::getInstance().get());
}

void QtMainWindow::find()
{
	MessageFind().dispatch();
}

void QtMainWindow::findFulltext()
{
	MessageFind(true).dispatch();
}

void QtMainWindow::overview()
{
	MessageSearch(std::vector<SearchMatch>(1, SearchMatch::createCommand(SearchMatch::COMMAND_ALL))).dispatch();
}

void QtMainWindow::closeWindow()
{
	QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());

	QWidget* activeWindow = app->activeWindow();
	if (activeWindow)
	{
		activeWindow->close();
	}
}

void QtMainWindow::refresh()
{
	MessageRefresh().dispatch();
}

void QtMainWindow::forceRefresh()
{
	MessageRefresh().refreshAll().dispatch();
}

void QtMainWindow::undo()
{
	MessageUndo().dispatch();
}

void QtMainWindow::redo()
{
	MessageRedo().dispatch();
}

void QtMainWindow::zoomIn()
{
	MessageZoom(true).dispatch();
}

void QtMainWindow::zoomOut()
{
	MessageZoom(false).dispatch();
}

void QtMainWindow::resetZoom()
{
	MessageResetZoom().dispatch();
}

void QtMainWindow::toggleView(View* view, bool fromMenu)
{
	DockWidget* dock = getDockWidgetForView(view);

	if (fromMenu)
	{
		dock->widget->setVisible(dock->action->isChecked());
	}
	else
	{
		dock->action->setChecked(dock->widget->isVisible());
	}
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	if(!isTrial())
	{
		menu->addAction(tr("&New Project..."), this, SLOT(newProject()), QKeySequence::New);
		menu->addAction(tr("&Open Project..."), this, SLOT(openProject()), QKeySequence::Open);
	}

	QMenu *recentProjectMenu = new QMenu(tr("Recent Projects"));
	menu->addMenu(recentProjectMenu);

	for (int i = 0; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount(); ++i)
	{
		m_recentProjectAction[i] = new QAction(this);
		m_recentProjectAction[i]->setVisible(false);
		connect(m_recentProjectAction[i], SIGNAL(triggered()),
			this, SLOT(openRecentProject()));
		recentProjectMenu->addAction(m_recentProjectAction[i]);
	}
	updateRecentProjectMenu();

	menu->addMenu(recentProjectMenu);

	menu->addSeparator();

	if(!isTrial())
	{
		menu->addAction(tr("&Edit Project..."), this, SLOT(editProject()));

		menu->addSeparator();
	}

	menu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::openRecentProject()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		openProject(action->data().toString());
	}
}

void QtMainWindow::updateRecentProjectMenu()
{
	std::vector<FilePath> recentProjects = ApplicationSettings::getInstance()->getRecentProjects();
	for (int i = 0; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount(); i++)
	{
		if ((size_t)i < recentProjects.size() && recentProjects[i].exists())
		{
			FilePath project = recentProjects[i];
			m_recentProjectAction[i]->setVisible(true);
			m_recentProjectAction[i]->setText(FileSystem::fileName(project.str()).c_str());
			m_recentProjectAction[i]->setData(project.str().c_str());
		}
		else
		{
			m_recentProjectAction[i]->setVisible(false);
		}
	}
}

void QtMainWindow::toggleShowDockWidgetTitleBars()
{
	setShowDockWidgetTitleBars(!m_showDockWidgetTitleBars);
}

void QtMainWindow::setupEditMenu()
{
	QMenu *menu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Back"), this, SLOT(undo()), QKeySequence::Undo);
	menu->addAction(tr("Forward"), this, SLOT(redo()), QKeySequence::Redo);

	menu->addSeparator();
	if(!isTrial())
	{
		menu->addAction(tr("&Refresh"), this, SLOT(refresh()), QKeySequence::Refresh);
		if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
		{
			menu->addAction(tr("&Force Refresh"), this, SLOT(forceRefresh()), QKeySequence(Qt::SHIFT + Qt::Key_F5));
		}
		else
		{
			menu->addAction(
				tr("&Force Refresh"),
				this,
				SLOT(forceRefresh()),
				QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_R)
			);
		}
	}

	menu->addSeparator();

	menu->addAction(tr("&Find Symbol"), this, SLOT(find()), QKeySequence::Find);
	menu->addAction(tr("&Find Text"), this, SLOT(findFulltext()), QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_F));

	menu->addSeparator();

	menu->addAction(tr("&To overview"), this, SLOT(overview()), QKeySequence::MoveToStartOfDocument);

	menu->addSeparator();

	menu->addAction(tr("Preferences..."), this, SLOT(openSettings()), QKeySequence(Qt::CTRL + Qt::Key_Comma));
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	m_showTitleBarsAction = new QAction("Show Title Bars", this);
	m_showTitleBarsAction->setCheckable(true);
	m_showTitleBarsAction->setChecked(m_showDockWidgetTitleBars);
	connect(m_showTitleBarsAction, SIGNAL(triggered()), this, SLOT(toggleShowDockWidgetTitleBars()));
	menu->addAction(m_showTitleBarsAction);

	menu->addSeparator();

	m_viewSeparator = menu->addSeparator();

	menu->addAction(tr("Larger font"), this, SLOT(zoomIn()), QKeySequence::ZoomIn);
	menu->addAction(tr("Smaller font"), this, SLOT(zoomOut()), QKeySequence::ZoomOut);
	menu->addAction(tr("Reset font size"), this, SLOT(resetZoom()), QKeySequence(Qt::CTRL + Qt::Key_0));

	m_viewMenu = menu;
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("Documentation"), this, SLOT(showDocumentation()));
	menu->addAction(tr("Bug Tracker"), this, SLOT(showBugtracker()));
	menu->addAction(tr("Licences"), this, SLOT(showLicenses()));

	if(!isTrial())
	{
		menu->addAction(tr("Enter License..."), this, SLOT(enterLicense()));
	}
}

QtMainWindow::DockWidget* QtMainWindow::getDockWidgetForView(View* view)
{
	for (DockWidget& dock : m_dockWidgets)
	{
		if (dock.view == view)
		{
			return &dock;
		}

		const CompositeView* compositeView = dynamic_cast<const CompositeView*>(dock.view);
		if (compositeView)
		{
			for (const View* v : compositeView->getViews())
			{
				if (v == view)
				{
					return &dock;
				}
			}
		}
	}

	LOG_ERROR("DockWidget was not found for view.");
	return nullptr;
}

void QtMainWindow::setShowDockWidgetTitleBars(bool showTitleBars)
{
	m_showDockWidgetTitleBars = showTitleBars;

	if (m_showTitleBarsAction)
	{
		m_showTitleBarsAction->setChecked(showTitleBars);
	}

	for (DockWidget& dock : m_dockWidgets)
	{
		if (showTitleBars)
		{
			dock.widget->setFeatures(QDockWidget::AllDockWidgetFeatures);
			dock.widget->setTitleBarWidget(nullptr);
		}
		else
		{
			dock.widget->setFeatures(QDockWidget::NoDockWidgetFeatures);
			dock.widget->setTitleBarWidget(new QWidget());
		}
	}
}

template<typename T>
	T* QtMainWindow::createWindow()
{
	T* window = new T(this);

	connect(window, SIGNAL(canceled()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(finished()), &m_windowStack, SLOT(clearWindows()));

	m_windowStack.pushWindow(window);

	return window;
}
