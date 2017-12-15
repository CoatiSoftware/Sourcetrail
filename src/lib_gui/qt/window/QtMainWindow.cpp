#include "qt/window/QtMainWindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDockWidget>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include "Application.h"
#include "component/view/CompositeView.h"
#include "component/view/TabbedView.h"
#include "component/view/View.h"
#include "data/bookmark/Bookmark.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/QtFileDialog.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "qt/window/project_wizzard/QtProjectWizzard.h"
#include "qt/window/QtAbout.h"
#include "qt/window/QtAboutLicense.h"
#include "qt/window/QtEulaWindow.h"
#include "qt/window/QtKeyboardShortcuts.h"
#include "qt/window/QtLicenseWindow.h"
#include "qt/window/QtPreferencesWindow.h"
#include "qt/window/QtStartScreen.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageCodeReference.h"
#include "utility/messaging/type/MessageDisplayBookmarkCreator.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageResetZoom.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageShowErrorHelpMessage.h"
#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageWindowClosed.h"
#include "utility/messaging/type/MessageZoom.h"
#include "utility/ResourcePaths.h"
#include "utility/tracing.h"
#include "utility/UserPaths.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"


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


QtMainWindow::QtMainWindow()
	: m_loaded(false)
	, m_historyMenu(nullptr)
	, m_bookmarksMenu(nullptr)
	, m_showDockWidgetTitleBars(true)
	, m_windowStack(this)
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setWindowIcon(QIcon((ResourcePaths::getGuiPath().str() + "icon/logo_1024_1024.png").c_str()));
	setWindowFlags(Qt::Widget);

#ifdef __linux__
	if (std::getenv("SOURCETRAIL_VIA_SCRIPT") == nullptr)
	{
	   QMessageBox::warning(this, "Run Sourcetrail via Script", "Please run Sourcetrail via Sourcetrail.sh");
	}
#endif

	QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());
	app->installEventFilter(new MouseReleaseFilter(this));

	refreshStyle();

	if (utility::getOsType() != OS_MAC)
	{
		// can only be done once, because resetting the style on the QCoreApplication causes crash
		app->setStyleSheet(
			utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("scrollbar.css"))).c_str());
	}

	m_recentProjectAction = new QAction*[ApplicationSettings::getInstance()->getMaxRecentProjectsCount()];

	setupProjectMenu();
	setupEditMenu();
	setupViewMenu();
	setupHistoryMenu();
	setupBookmarksMenu();
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

	// Disable un-intended vertical growth of search widget
	if (view->getName() == "Search")
	{
		dock->setSizePolicy(dock->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	}

	if (!m_showDockWidgetTitleBars)
	{
		dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		dock->setTitleBarWidget(new QWidget());
	}

	addDockWidget(Qt::TopDockWidgetArea, dock);

	QtViewToggle* toggle = new QtViewToggle(view, this);
	connect(dock, &QDockWidget::visibilityChanged, toggle, &QtViewToggle::toggledByUI);

	QAction* action = new QAction(tr((view->getName() + " Window").c_str()), this);
	action->setCheckable(true);
	connect(action, &QAction::triggered, toggle, &QtViewToggle::toggledByAction);
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

View* QtMainWindow::findFloatingView(const std::string& name) const
{
	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		if (std::string(m_dockWidgets[i].view->getName()) == name && m_dockWidgets[i].widget->isFloating())
		{
			return m_dockWidgets[i].view;
		}
	}

	return nullptr;
}

void QtMainWindow::loadLayout()
{
	QSettings settings(UserPaths::getWindowSettingsPath().str().c_str(), QSettings::IniFormat);

	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(600, 400)).toSize());
	move(settings.value("position", QPoint(200, 200)).toPoint());
	if (settings.value("maximized", false).toBool())
	{
		showMaximized();
	}
	setShowDockWidgetTitleBars(settings.value("showTitleBars", true).toBool());
	settings.endGroup();
	loadDockWidgetLayout();
}

void QtMainWindow::loadDockWidgetLayout()
{
	QSettings settings(UserPaths::getWindowSettingsPath().str().c_str(), QSettings::IniFormat);
	this->restoreState(settings.value("DOCK_LOCATIONS").toByteArray());

	for (DockWidget dock : m_dockWidgets)
	{
		dock.action->setChecked(!dock.widget->isHidden());
	}
}

void QtMainWindow::loadWindow(bool showStartWindow)
{
	if (m_loaded)
	{
		if (!showStartWindow)
		{
			hideStartScreen();
		}
		return;
	}

	m_loaded = true;

	LicenseChecker::LicenseState state = LicenseChecker::getInstance()->checkCurrentLicense();
	bool licenseValid = (state == LicenseChecker::LICENSE_VALID);

	if (licenseValid)
	{
		MessageEnteredLicense(LicenseChecker::getInstance()->getCurrentLicenseType()).dispatch();
	}

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (state == LicenseChecker::LICENSE_MOVED)
	{
		appSettings->setLicenseString("");
	}

	if (showStartWindow)
	{
		showStartScreen();
	}

	if (state != LicenseChecker::LICENSE_VALID && !appSettings->getNonCommercialUse())
	{
		forceEnterLicense(state);
	}

#ifndef Q_OS_WIN
	if (appSettings->getAcceptedEulaVersion() < QtEulaWindow::EULA_VERSION)
	{
		showEula(true);
	}
#endif
}

void QtMainWindow::saveLayout()
{
	QSettings settings(UserPaths::getWindowSettingsPath().str().c_str(), QSettings::IniFormat);

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

void QtMainWindow::forceEnterLicense(LicenseChecker::LicenseState state)
{
	QtLicenseWindow* window = dynamic_cast<QtLicenseWindow*>(m_windowStack.getTopWindow());
	if (window)
	{
		return;
	}

	enterLicense();

	window = dynamic_cast<QtLicenseWindow*>(m_windowStack.getTopWindow());
	if (!window)
	{
		LOG_ERROR("No enter license window on top of stack");
		return;
	}

	if (state == LicenseChecker::LICENSE_EXPIRED)
	{
		window->setErrorMessage("The license key is expired.");
	}
	else if (state != LicenseChecker::LICENSE_VALID && state != LicenseChecker::LICENSE_EMPTY)
	{
		window->clear();
		window->setErrorMessage("Please re-enter your license key.");
	}

	window->updateCloseButton("Quit");

	setEnabled(false);
	window->setEnabled(true);

	disconnect(window, &QtWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtWindow::canceled, dynamic_cast<QApplication*>(QCoreApplication::instance()), &QApplication::quit);
}

void QtMainWindow::updateHistoryMenu(const std::vector<SearchMatch>& history)
{
	m_history = history;
	setupHistoryMenu();
}

void QtMainWindow::updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_bookmarks = bookmarks;
	setupBookmarksMenu();
}

void QtMainWindow::setContentEnabled(bool enabled)
{
	foreach (QAction *action, menuBar()->actions())
	{
		action->setEnabled(enabled);
	}

	for (DockWidget& dock : m_dockWidgets)
	{
		dock.widget->setEnabled(enabled);
	}
}

void QtMainWindow::refreshStyle()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("main.css"))).c_str());
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
			emit hideScreenSearch();
			break;

		case Qt::Key_Slash:
		case Qt::Key_Question:
			emit showScreenSearch();
			break;

		case Qt::Key_Space:
			PRINT_TRACES();
			break;
	}
}

void QtMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu menu(event, this);
	menu.show();
}

void QtMainWindow::closeEvent(QCloseEvent* event)
{
	MessageWindowClosed().dispatchImmediately();
}

void QtMainWindow::resizeEvent(QResizeEvent *event)
{
	m_windowStack.centerSubWindows();
	QMainWindow::resizeEvent(event);
}

void QtMainWindow::about()
{
	QtAbout* aboutWindow = createWindow<QtAbout>();
	aboutWindow->setupAbout();
}

void QtMainWindow::openSettings()
{
	QtPreferencesWindow* window = createWindow<QtPreferencesWindow>();
	window->setup();
}

void QtMainWindow::showDocumentation()
{
	QDesktopServices::openUrl(QUrl("https://sourcetrail.com/documentation/"));
}

void QtMainWindow::showKeyboardShortcuts()
{
	QtKeyboardShortcuts* keyboardShortcutWindow = createWindow<QtKeyboardShortcuts>();
	keyboardShortcutWindow->setup();
}

void QtMainWindow::showErrorHelpMessage()
{
	MessageShowErrorHelpMessage(true).dispatch();
}

void QtMainWindow::showBugtracker()
{
	QDesktopServices::openUrl(QUrl("https://github.com/CoatiSoftware/SourcetrailBugTracker/issues"));
}

void QtMainWindow::showEula(bool forceAccept)
{
	QtEulaWindow* window = new QtEulaWindow(this, forceAccept);
	m_windowStack.pushWindow(window);
	window->setup();

	if (forceAccept)
	{
		setEnabled(false);
		window->setEnabled(true);

		connect(window, &QtEulaWindow::finished, this, &QtMainWindow::acceptedEula);
		connect(window, &QtEulaWindow::canceled, dynamic_cast<QApplication*>(QCoreApplication::instance()), &QApplication::quit);
	}
	else
	{
		connect(window, &QtEulaWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
	}
}

void QtMainWindow::acceptedEula()
{
	ApplicationSettings::getInstance()->setAcceptedEulaVersion(QtEulaWindow::EULA_VERSION);
	ApplicationSettings::getInstance()->save();

	setEnabled(true);
	m_windowStack.popWindow();
}

void QtMainWindow::showLicenses()
{
	QtAboutLicense* licenseWindow = createWindow<QtAboutLicense>();
	licenseWindow->setup();
}

void QtMainWindow::enterLicense()
{
	QtLicenseWindow* enterLicenseWindow = createWindow<QtLicenseWindow>();
	enterLicenseWindow->setup();

	disconnect(enterLicenseWindow, &QtLicenseWindow::finished, &m_windowStack, &QtWindowStack::clearWindows);
	connect(enterLicenseWindow, &QtLicenseWindow::finished, this, &QtMainWindow::enteredLicense);

	enterLicenseWindow->load();
}

void QtMainWindow::enteredLicense()
{
	bool showStartWindow = false;
	if (m_windowStack.getWindowCount() > 0 && dynamic_cast<QtStartScreen*>(m_windowStack.getBottomWindow()))
	{
		showStartWindow = true;
	}

	m_windowStack.clearWindows();

	MessageEnteredLicense(LicenseChecker::getInstance()->getCurrentLicenseType()).dispatch();

	setEnabled(true);

	if (showStartWindow)
	{
		showStartScreen();
	}
}

void QtMainWindow::showDataFolder()
{
	QDesktopServices::openUrl(QUrl(("file:///" + UserPaths::getUserDataPath().canonical().str()).c_str(), QUrl::TolerantMode));
}

void QtMainWindow::showLogFolder()
{
	QDesktopServices::openUrl(QUrl(("file:///" + UserPaths::getLogPath().canonical().str()).c_str(), QUrl::TolerantMode));
}

void QtMainWindow::showStartScreen()
{
	if (dynamic_cast<QtStartScreen*>(m_windowStack.getTopWindow()))
	{
		return;
	}

	QtStartScreen* startScreen = createWindow<QtStartScreen>();
	startScreen->setupStartScreen();

	connect(startScreen, &QtStartScreen::openOpenProjectDialog, this, &QtMainWindow::openProject);
	connect(startScreen, &QtStartScreen::openNewProjectDialog, this, &QtMainWindow::newProject);
	connect(startScreen, &QtStartScreen::openEnterLicenseDialog, this, &QtMainWindow::enterLicense);
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

void QtMainWindow::newProjectFromCDB(const std::string& filePath, const std::vector<std::string>& headerPaths)
{
	QtProjectWizzard* wizzard = dynamic_cast<QtProjectWizzard*>(m_windowStack.getTopWindow());
	if (!wizzard)
	{
		wizzard = createWindow<QtProjectWizzard>();
	}

	std::vector<FilePath> headerFilePaths;
	for (const std::string& s: headerPaths)
	{
		headerFilePaths.push_back(FilePath(s));
	}

	wizzard->newProjectFromCDB(FilePath(filePath), headerFilePaths);
}

void QtMainWindow::openProject()
{
	QString fileName = QtFileDialog::getOpenFileName(
		this, tr("Open File"), QDir::homePath(), "Sourcetrail Project Files (*.srctrlprj *.coatiproject)");

	if (!fileName.isEmpty())
	{
		MessageLoadProject(FilePath(fileName.toStdString())).dispatch();
		m_windowStack.clearWindows();
	}
}

void QtMainWindow::editProject()
{
	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (currentProject)
	{
		QtProjectWizzard* wizzard = createWindow<QtProjectWizzard>();

		wizzard->editProject(currentProject->getProjectSettingsFilePath());
	}
}

void QtMainWindow::find()
{
	MessageFind().dispatch();
}

void QtMainWindow::findFulltext()
{
	MessageFind(true).dispatch();
}

void QtMainWindow::findOnScreen()
{
	emit showScreenSearch();
}

void QtMainWindow::codeReferencePrevious()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_PREVIOUS).dispatch();
}

void QtMainWindow::codeReferenceNext()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_NEXT).dispatch();
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
	if (Qt::KeyboardModifier::AltModifier & QApplication::keyboardModifiers())
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}
	else
	{
		MessageRefresh().dispatch();
	}
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

void QtMainWindow::resetWindowLayout()
{
	FileSystem::remove(UserPaths::getWindowSettingsPath());
	FileSystem::copyFile(
		ResourcePaths::getFallbackPath().concat(FilePath("window_settings.ini")), UserPaths::getWindowSettingsPath());
	loadDockWidgetLayout();
}

void QtMainWindow::openRecentProject()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
	{
		MessageLoadProject(FilePath(action->data().toString().toStdString())).dispatch();
		m_windowStack.clearWindows();
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

void QtMainWindow::toggleShowDockWidgetTitleBars()
{
	setShowDockWidgetTitleBars(!m_showDockWidgetTitleBars);
}

void QtMainWindow::showBookmarkCreator()
{
	MessageDisplayBookmarkCreator().dispatch();
}

void QtMainWindow::showBookmarkBrowser()
{
	MessageDisplayBookmarks().dispatch();
}

void QtMainWindow::openHistoryAction()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		SearchMatch& match = m_history[action->data().toInt()];

		MessageSearch msg({ match });
		msg.isFromSearch = false;
		msg.dispatch();
	}
}

void QtMainWindow::activateBookmarkAction()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		std::shared_ptr<Bookmark> bookmark = m_bookmarks[action->data().toInt()];
		MessageActivateBookmark(bookmark).dispatch();
	}
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&New Project..."), this, &QtMainWindow::newProject, QKeySequence::New);
	menu->addAction(tr("&Open Project..."), this, &QtMainWindow::openProject, QKeySequence::Open);

	QMenu *recentProjectMenu = new QMenu(tr("Recent Projects"));
	menu->addMenu(recentProjectMenu);

	for (int i = 0; i < ApplicationSettings::getInstance()->getMaxRecentProjectsCount(); ++i)
	{
		m_recentProjectAction[i] = new QAction(this);
		m_recentProjectAction[i]->setVisible(false);
		connect(m_recentProjectAction[i], &QAction::triggered,
			this, &QtMainWindow::openRecentProject);
		recentProjectMenu->addAction(m_recentProjectAction[i]);
	}
	updateRecentProjectMenu();

	menu->addMenu(recentProjectMenu);

	menu->addSeparator();

	menu->addAction(tr("&Edit Project..."), this, &QtMainWindow::editProject);
	menu->addSeparator();

	menu->addAction(tr("E&xit"), QCoreApplication::instance(), &QCoreApplication::quit, QKeySequence::Quit);
}

void QtMainWindow::setupEditMenu()
{
	QMenu *menu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&Refresh"), this, &QtMainWindow::refresh, QKeySequence::Refresh);
	if (utility::getOsType() == OS_WINDOWS)
	{
		menu->addAction(tr("&Full Refresh"), this, &QtMainWindow::forceRefresh, QKeySequence(Qt::SHIFT + Qt::Key_F5));
	}
	else
	{
		menu->addAction(tr("&Full Refresh"), this, &QtMainWindow::forceRefresh, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_R));
	}

	menu->addSeparator();

	menu->addAction(tr("&Find Symbol"), this, &QtMainWindow::find, QKeySequence::Find);
	menu->addAction(tr("&Find Text"), this, &QtMainWindow::findFulltext, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_F));
	menu->addAction(tr("&Find On-Screen"), this, &QtMainWindow::findOnScreen, QKeySequence(Qt::CTRL + Qt::Key_D));

	menu->addSeparator();

	menu->addAction(tr("Code Reference Next"), this, &QtMainWindow::codeReferenceNext, QKeySequence(Qt::CTRL + Qt::Key_G));
	menu->addAction(tr("Code Reference Previous"), this,
		&QtMainWindow::codeReferencePrevious, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_G));

	menu->addSeparator();

	menu->addAction(tr("&To overview"), this, &QtMainWindow::overview, QKeySequence::MoveToStartOfDocument);

	menu->addSeparator();

	menu->addAction(tr("Preferences..."), this, &QtMainWindow::openSettings, QKeySequence(Qt::CTRL + Qt::Key_Comma));
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Show Start Window"), this, &QtMainWindow::showStartScreen);

	m_showTitleBarsAction = new QAction("Show Title Bars", this);
	m_showTitleBarsAction->setCheckable(true);
	m_showTitleBarsAction->setChecked(m_showDockWidgetTitleBars);
	connect(m_showTitleBarsAction, &QAction::triggered, this, &QtMainWindow::toggleShowDockWidgetTitleBars);
	menu->addAction(m_showTitleBarsAction);

	menu->addSeparator();

	m_viewSeparator = menu->addSeparator();

	menu->addAction(tr("Larger font"), this, &QtMainWindow::zoomIn, QKeySequence::ZoomIn);
	menu->addAction(tr("Smaller font"), this, &QtMainWindow::zoomOut, QKeySequence::ZoomOut);
	menu->addAction(tr("Reset font size"), this, &QtMainWindow::resetZoom, QKeySequence(Qt::CTRL + Qt::Key_0));
	menu->addAction(tr("Reset window layout"), this, &QtMainWindow::resetWindowLayout);

	m_viewMenu = menu;
}

void QtMainWindow::setupHistoryMenu()
{
	if (!m_historyMenu)
	{
		m_historyMenu = new QMenu(tr("&History"), this);
		menuBar()->addMenu(m_historyMenu);
	}
	else
	{
		m_historyMenu->clear();
	}

	m_historyMenu->addAction(tr("Back"), this, &QtMainWindow::undo, QKeySequence::Undo);
	m_historyMenu->addAction(tr("Forward"), this, &QtMainWindow::redo, QKeySequence::Redo);

	m_historyMenu->addSeparator();

	QAction* title = new QAction(tr("Recently Active Symbols"));
	title->setEnabled(false);
	m_historyMenu->addAction(title);

	for (size_t i = 0; i < m_history.size(); i++)
	{
		SearchMatch& match = m_history[i];
		std::string name = utility::elide(match.nodeType.isFile() ? match.text : match.name, utility::ELIDE_RIGHT, 50);

		QAction* action = new QAction();
		action->setText(name.c_str());
		action->setData(QVariant(int(i)));

		connect(action, &QAction::triggered, this, &QtMainWindow::openHistoryAction);
		m_historyMenu->addAction(action);
	}
}

void QtMainWindow::setupBookmarksMenu()
{
	if (!m_bookmarksMenu)
	{
		m_bookmarksMenu = new QMenu(tr("&Bookmarks"), this);
		menuBar()->addMenu(m_bookmarksMenu);
	}
	else
	{
		m_bookmarksMenu->clear();
	}

	m_bookmarksMenu->addAction(tr("Bookmark Active Symbol..."), this, &QtMainWindow::showBookmarkCreator, QKeySequence::Save);
	m_bookmarksMenu->addAction(tr("Bookmark Manager"), this,
		&QtMainWindow::showBookmarkBrowser, QKeySequence(Qt::CTRL + Qt::Key_B));

	m_bookmarksMenu->addSeparator();

	QAction* title = new QAction(tr("Recent Bookmarks"));
	title->setEnabled(false);
	m_bookmarksMenu->addAction(title);

	for (size_t i = 0; i < m_bookmarks.size(); i++)
	{
		Bookmark* bookmark = m_bookmarks[i].get();
		std::string name = utility::elide(bookmark->getName(), utility::ELIDE_RIGHT, 50);

		QAction* action = new QAction();
		action->setText(name.c_str());
		action->setData(QVariant(int(i)));

		connect(action, &QAction::triggered, this, &QtMainWindow::activateBookmarkAction);
		m_bookmarksMenu->addAction(action);
	}
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Keyboard Shortcuts"), this, &QtMainWindow::showKeyboardShortcuts);
	menu->addAction(tr("Fixing Errors"), this, &QtMainWindow::showErrorHelpMessage);
	menu->addAction(tr("Documentation"), this, &QtMainWindow::showDocumentation);
	menu->addAction(tr("Bug Tracker"), this, &QtMainWindow::showBugtracker);

	menu->addSeparator();

	menu->addAction(tr("Select License..."), this, &QtMainWindow::enterLicense);
	menu->addAction(tr("End User License Agreement"), this, &QtMainWindow::showEula);
	menu->addAction(tr("3rd Party Licenses"), this, &QtMainWindow::showLicenses);
	menu->addAction(tr("&About Sourcetrail"), this, &QtMainWindow::about);

	menu->addSeparator();

	menu->addAction(tr("Show Data Folder"), this, &QtMainWindow::showDataFolder);
	menu->addAction(tr("Show Log Folder"), this, &QtMainWindow::showLogFolder);
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

		const TabbedView* tabbedView = dynamic_cast<const TabbedView*>(dock.view);
		if (tabbedView)
		{
			for (const View* v : tabbedView->getViews())
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

	connect(window, &QtWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtWindow::finished, &m_windowStack, &QtWindowStack::clearWindows);

	m_windowStack.pushWindow(window);

	return window;
}
