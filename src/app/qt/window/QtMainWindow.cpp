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
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ApplicationSettings.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSaveProject.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "utility/messaging/type/MessageZoom.h"
#include "version.h"


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


QtMainWindow::QtMainWindow()
	:  m_startScreenWasVisible(false)
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setWindowIcon(QIcon("./data/gui/icon/logo_1024_1024.png"));
	setWindowFlags(Qt::Widget);

	QApplication::setOverrideCursor(Qt::ArrowCursor);

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
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (appSettings->getUserHasSeenSettings())
	{
		showStartScreen();
	}
	else
	{
		openSettings();
		m_startScreenWasVisible = true;

		appSettings->setUserHasSeenSettings(true);
		appSettings->save();
	}
}

QtMainWindow::~QtMainWindow()
{
}

void QtMainWindow::addView(View* view)
{
	QDockWidget* dock = new QDockWidget(tr(view->getName().c_str()), this);
	dock->setWidget(QtViewWidgetWrapper::getWidgetOfView(view));
	dock->setObjectName(QString::fromStdString("Dock" + view->getName()));
	//dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    //dock->setTitleBarWidget(new QWidget());
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
	QSettings settings("data/window_settings.ini", QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(600, 400)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());
	if (settings.value("maximized", false).toBool())
	{
		showMaximized();
	}
	settings.endGroup();

	this->restoreState(settings.value("DOCK_LOCATIONS").toByteArray());

	for (DockWidget dock : m_dockWidgets)
	{
		dock.action->setChecked(!dock.widget->isHidden());
	}
}

void QtMainWindow::saveLayout()
{
	QSettings settings("data/window_settings.ini", QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	settings.setValue("maximized", isMaximized());
	if (!isMaximized())
	{
		settings.setValue("size", size());
		settings.setValue("position", pos());
	}
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

void QtMainWindow::about()
{
	hideScreens();
	m_aboutWindow = std::make_shared<QtAbout>(this);
	m_aboutWindow->setup();
	m_aboutWindow->show();
}

void QtMainWindow::hideScreens()
{
	if (m_applicationSettingsScreen)
	{
		m_applicationSettingsScreen->hide();
	}

	if (m_startScreen)
	{
		m_startScreen->hide();
	}

	if (m_newProjectDialog)
	{
		m_newProjectDialog->hide();
	}

	if(m_licenseWindow)
	{
		m_licenseWindow->hide();
	}

	if(m_aboutWindow)
	{
		m_aboutWindow->hide();
	}
}

void QtMainWindow::closeScreens()
{
	hideScreens();
	m_startScreenWasVisible = false;
}

void QtMainWindow::restoreScreens()
{
	if (m_startScreenWasVisible)
	{
		showStartScreen();
	}
	else
	{
		closeScreens();
	}
}

void QtMainWindow::openSettings()
{
	hideScreens();

	if (!m_applicationSettingsScreen)
	{
		m_applicationSettingsScreen = std::make_shared<QtApplicationSettingsScreen>(this);
		m_applicationSettingsScreen->setup();
		connect(m_applicationSettingsScreen.get(), SIGNAL(finished()), this, SLOT(restoreScreens()));
		connect(m_applicationSettingsScreen.get(), SIGNAL(canceled()), this, SLOT(restoreScreens()));
	}

	m_applicationSettingsScreen->load();
	m_applicationSettingsScreen->show();
}

void QtMainWindow::showStartScreen()
{
	hideScreens();

	if (!m_startScreen)
	{
		m_startScreen = std::make_shared<QtStartScreen>(this);
		m_startScreen->setup();
		connect(m_startScreen.get(), SIGNAL(finished()), this, SLOT(closeScreens()));
		connect(m_startScreen.get(), SIGNAL(canceled()), this, SLOT(closeScreens()));

		connect(m_startScreen.get(), SIGNAL(openOpenProjectDialog()), this, SLOT(openProject()));
		connect(m_startScreen.get(), SIGNAL(openNewProjectDialog()), this, SLOT(newProject()));
	}

	m_startScreen->show();
	m_startScreenWasVisible = true;
}

void QtMainWindow::newProject()
{
	hideScreens();

	if (!m_newProjectDialog)
	{
		m_newProjectDialog = std::make_shared<QtProjectSetupScreen>(this);
		m_newProjectDialog->setup();

		connect(m_newProjectDialog.get(), SIGNAL(finished()), this, SLOT(closeScreens()));
		connect(m_newProjectDialog.get(), SIGNAL(canceled()), this, SLOT(restoreScreens()));
	}

	m_newProjectDialog->loadEmpty();
	m_newProjectDialog->show();
}

void QtMainWindow::openProject(const QString &path)
{
	QString fileName = path;

	if (fileName.isNull())
	{
		fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "XML Files (*.xml)");
	}

	if (!fileName.isEmpty())
	{
		MessageLoadProject(fileName.toStdString()).dispatch();
		closeScreens();
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
	filename = QFileDialog::getSaveFileName(this, "Save File as", "", "XML Files(*.xml)");

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
	closeScreens();
	MessageInterruptTasks().dispatch();
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&New Project..."), this, SLOT(newProject()), QKeySequence::New);
	menu->addAction(tr("&Open Project..."), this, SLOT(openProject()), QKeySequence::Open);
	menu->addAction(tr("&Edit Project..."), this, SLOT(editProject()));

	menu->addSeparator();

	menu->addAction(tr("&Save Project"), this, SLOT(saveProject()), QKeySequence::Save);
	menu->addAction(tr("Save Project as..."), this, SLOT(saveAsProject()), QKeySequence::SaveAs);

	menu->addSeparator();

	menu->addAction(tr("&Close Window"), this, SLOT(closeWindow()), QKeySequence::Close);
	menu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::showLicenses()
{
	hideScreens();
	m_licenseWindow = std::make_shared<QtAboutLicense>(this);
	m_licenseWindow->setup();
	m_licenseWindow->show();
}

void QtMainWindow::setupEditMenu()
{
	QMenu *menu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Undo"), this, SLOT(undo()), QKeySequence::Undo);
	menu->addAction(tr("Redo"), this, SLOT(redo()), QKeySequence::Redo);

	menu->addSeparator();

	menu->addAction(tr("&Refresh"), this, SLOT(refresh()), QKeySequence::Refresh);

	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		menu->addAction(tr("&Force Refresh"), this, SLOT(forceRefresh()), QKeySequence(Qt::SHIFT + Qt::Key_F5));
	}
	else
	{
		menu->addAction(tr("&Force Refresh"), this, SLOT(forceRefresh()), QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_R));
	}

	menu->addAction(tr("&Find"), this, SLOT(find()), QKeySequence::Find);
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	m_viewSeparator = menu->addSeparator();

	menu->addAction(tr("Larger font"), this, SLOT(zoomIn()), QKeySequence::ZoomIn);
	menu->addAction(tr("Smaller font"), this, SLOT(zoomOut()), QKeySequence::ZoomOut);

	menu->addAction(tr("Switch Color Scheme..."), this, SLOT(switchColorScheme()));

	m_viewMenu = menu;
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("Licences"), this, SLOT(showLicenses()));
	menu->addAction(tr("Preferences..."), this, SLOT(openSettings()));
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
