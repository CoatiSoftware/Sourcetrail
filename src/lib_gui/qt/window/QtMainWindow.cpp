#include "qt/window/QtMainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSysInfo>

#include "component/view/View.h"
#include "component/view/CompositeView.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageResetZoom.h"
#include "utility/messaging/type/MessageSaveProject.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "utility/messaging/type/MessageZoom.h"
#include "version.h"
#include "isTrial.h"

std::string QtMainWindow::m_windowSettingsPath = "data/window_settings.ini";

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
{

}

bool MouseWheelFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Wheel && QApplication::keyboardModifiers() == Qt::ControlModifier)
	{
		QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);

		if (wheelEvent->delta() > 0.0f)
		{
			MessageZoom(true).dispatch();
			return true;
		}
		else if (wheelEvent->delta() < 0.0f)
		{
			MessageZoom(false).dispatch();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

QtMainWindow::QtMainWindow()
	: m_showDockWidgetTitleBars(true)
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setWindowIcon(QIcon("./data/gui/icon/logo_1024_1024.png"));
	setWindowFlags(Qt::Widget);

	QApplication::setOverrideCursor(Qt::ArrowCursor);

	QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());
	app->installEventFilter(new MouseReleaseFilter(this));
	app->installEventFilter(new MouseWheelFilter(this));

	app->setStyleSheet(utility::getStyleSheet("data/gui/tooltip.css").c_str());

	m_recentProjectAction = new QAction*[ApplicationSettings::MaximalAmountOfRecentProjects];

	setupProjectMenu();
	setupEditMenu();
	setupViewMenu();
	setupHelpMenu();

	setupShortcuts();

	// Need to call loadLayout here for right DockWidget size on Linux
	// Seconde call is in Application.cpp
	loadLayout();
}

void QtMainWindow::init()
{
	showStartScreen();
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
	QSettings settings(m_windowSettingsPath.c_str(), QSettings::IniFormat);

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
	QSettings settings(m_windowSettingsPath.c_str(), QSettings::IniFormat);

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
	if (event->key() == Qt::Key_Backspace)
	{
		MessageUndo().dispatch();
	}
}

void QtMainWindow::pushWindow(QWidget* window)
{
	if (m_windowStack.size())
	{
		m_windowStack.back()->hide();
	}

	window->show();

	m_windowStack.push_back(window);
}

void QtMainWindow::popWindow()
{
	if (m_windowStack.size())
	{
		m_windowStack.back()->hide();
		m_windowStack.pop_back();
	}

	if (m_windowStack.size())
	{
		m_windowStack.back()->show();
	}
}

void QtMainWindow::clearWindows()
{
	if (m_windowStack.size())
	{
		m_windowStack.back()->hide();
	}

	m_windowStack.clear();
}

void QtMainWindow::about()
{
	if (!m_aboutWindow)
	{
		m_aboutWindow = std::make_shared<QtAbout>(this);
		m_aboutWindow->setup();

		connect(m_aboutWindow.get(), SIGNAL(finished()), this, SLOT(popWindow()));
	}

	pushWindow(m_aboutWindow.get());
}

void QtMainWindow::openSettings()
{
	if (!m_applicationSettingsScreen)
	{
		m_applicationSettingsScreen = std::make_shared<QtApplicationSettingsScreen>(this);
		m_applicationSettingsScreen->setup();

		connect(m_applicationSettingsScreen.get(), SIGNAL(finished()), this, SLOT(popWindow()));
		connect(m_applicationSettingsScreen.get(), SIGNAL(canceled()), this, SLOT(popWindow()));
	}

	m_applicationSettingsScreen->load();
	pushWindow(m_applicationSettingsScreen.get());
}

void QtMainWindow::showLicenses()
{
	if (!m_licenseWindow)
	{
		m_licenseWindow = std::make_shared<QtAboutLicense>(this);
		m_licenseWindow->setup();

		connect(m_licenseWindow.get(), SIGNAL(finished()), this, SLOT(popWindow()));
	}

	pushWindow(m_licenseWindow.get());
}

void QtMainWindow::showStartScreen()
{
	if (!m_startScreen)
	{
		m_startScreen = std::make_shared<QtStartScreen>(this);
		m_startScreen->setup();

		connect(m_startScreen.get(), SIGNAL(finished()), this, SLOT(popWindow()));
		connect(m_startScreen.get(), SIGNAL(canceled()), this, SLOT(popWindow()));

		connect(m_startScreen.get(), SIGNAL(openOpenProjectDialog()), this, SLOT(openProject()));
		connect(m_startScreen.get(), SIGNAL(openNewProjectDialog()), this, SLOT(newProject()));
	}

	pushWindow(m_startScreen.get());
}

void QtMainWindow::newProject()
{
	if (!m_newProjectDialog)
	{
		m_newProjectDialog = std::make_shared<QtProjectSetupScreen>(this);
		m_newProjectDialog->setup();

		connect(m_newProjectDialog.get(), SIGNAL(finished()), this, SLOT(clearWindows()));
		connect(m_newProjectDialog.get(), SIGNAL(canceled()), this, SLOT(popWindow()));
		connect(m_newProjectDialog.get(), SIGNAL(showPreferences()), this, SLOT(openSettings()));
	}

	m_newProjectDialog->loadEmpty();
	pushWindow(m_newProjectDialog.get());
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
		MessageLoadProject(fileName.toStdString()).dispatch();
		clearWindows();
	}
}

void QtMainWindow::editProject()
{
	newProject();
	m_newProjectDialog->loadProjectSettings();
}

void QtMainWindow::find()
{
	MessageFind().dispatch();
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

void QtMainWindow::saveProject()
{
	MessageSaveProject("").dispatch();
}

void QtMainWindow::saveAsProject()
{
	QString filename = "";
	filename = QFileDialog::getSaveFileName(this, "Save File as", "", "Coati Project Files(*.coatiproject)");

	if(!filename.isEmpty())
	{
		MessageSaveProject(filename.toStdString()).dispatch();
	}
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

void QtMainWindow::switchColorScheme()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "./data/color_schemes", "XML Files (*.xml)");

	if (!fileName.isEmpty())
	{
		MessageSwitchColorScheme(fileName.toStdString()).dispatch();
	}
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

void QtMainWindow::handleEscapeShortcut()
{
	popWindow();
	MessageInterruptTasks().dispatch();
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	if(!isTrial())
	{
		menu->addAction(tr("&New Project..."), this, SLOT(newProject()), QKeySequence::New);
		menu->addAction(tr("&Open Project..."), this, SLOT(openProject()), QKeySequence::Open);
		menu->addAction(tr("&Edit Project..."), this, SLOT(editProject()));

		menu->addSeparator();

		menu->addAction(tr("&Save Project"), this, SLOT(saveProject()), QKeySequence::Save);
		menu->addAction(tr("Save Project as..."), this, SLOT(saveAsProject()), QKeySequence::SaveAs);

		menu->addSeparator();
	}

	QMenu *recentProjectMenu = new QMenu(tr("Recent Projects"));
	menu->addMenu(recentProjectMenu);

	for (int i = 0; i < ApplicationSettings::MaximalAmountOfRecentProjects; ++i)
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
	for (size_t i = 0; i < ApplicationSettings::MaximalAmountOfRecentProjects; i++)
	{
		if(i < recentProjects.size())
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

void QtMainWindow::setWindowSettingsPath(const std::string& windowSettingsPath)
{
	m_windowSettingsPath = windowSettingsPath;
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
			menu->addAction(tr("&Force Refresh"), this, SLOT(forceRefresh()), QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_R));
		}
	}

	menu->addAction(tr("&Find"), this, SLOT(find()), QKeySequence::Find);
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

	menu->addSeparator();

	menu->addAction(tr("Switch Color Scheme..."), this, SLOT(switchColorScheme()));

	m_viewMenu = menu;
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("Licences"), this, SLOT(showLicenses()));
	if(!isTrial())
	{
		menu->addAction(tr("Preferences..."), this, SLOT(openSettings()));
		//Todo: Enter License Window
	}
}

void QtMainWindow::setupShortcuts()
{
	m_escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(m_escapeShortcut, SIGNAL(activated()), SLOT(handleEscapeShortcut()));
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
