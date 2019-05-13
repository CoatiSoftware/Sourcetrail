#include "QtMainWindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDockWidget>
#include <QDir>
#include <QMenuBar>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <QToolTip>

#include "Application.h"
#include "CompositeView.h"
#include "TabbedView.h"
#include "View.h"
#include "Bookmark.h"
#include "QtContextMenu.h"
#include "QtFileDialog.h"
#include "utilityQt.h"
#include "QtViewWidgetWrapper.h"
#include "QtProjectWizzard.h"
#include "QtAbout.h"
#include "QtAboutLicense.h"
#include "QtEulaWindow.h"
#include "QtKeyboardShortcuts.h"
#include "QtLicenseWindow.h"
#include "QtPreferencesWindow.h"
#include "QtStartScreen.h"
#include "ApplicationSettings.h"
#include "FileSystem.h"
#include "LicenseChecker.h"
#include "logging.h"
#include "MessageErrorsHelpMessage.h"
#include "MessageHistoryRedo.h"
#include "MessageHistoryUndo.h"
#include "MessageActivateAll.h"
#include "MessageActivateBase.h"
#include "MessageBookmarkActivate.h"
#include "MessageBookmarkBrowse.h"
#include "MessageBookmarkCreate.h"
#include "MessageCodeReference.h"
#include "MessageFind.h"
#include "MessageIndexingShowDialog.h"
#include "MessageLoadProject.h"
#include "MessageRefresh.h"
#include "MessageRefreshUI.h"
#include "MessageResetZoom.h"
#include "MessageTabClose.h"
#include "MessageTabOpen.h"
#include "MessageTabSelect.h"
#include "MessageWindowClosed.h"
#include "MessageZoom.h"
#include "ResourcePaths.h"
#include "tracing.h"
#include "UserPaths.h"
#include "utilityApp.h"
#include "utilityString.h"


QtViewToggle::QtViewToggle(View* view, QWidget *parent)
	: QWidget(parent)
	, m_view(view)
{
}

void QtViewToggle::clear()
{
	m_view = nullptr;
}

void QtViewToggle::toggledByAction()
{
	if (m_view)
	{
		dynamic_cast<QtMainWindow*>(parent())->toggleView(m_view, true);
	}
}

void QtViewToggle::toggledByUI()
{
	if (m_view)
	{
		dynamic_cast<QtMainWindow*>(parent())->toggleView(m_view, false);
	}
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
			MessageHistoryUndo().dispatch();
			return true;
		}
		else if (mouseEvent->button() == m_forwardButton)
		{
			MessageHistoryRedo().dispatch();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}


QtMainWindow::QtMainWindow()
	: m_historyMenu(nullptr)
	, m_bookmarksMenu(nullptr)
	, m_showDockWidgetTitleBars(true)
	, m_windowStack(this)
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setWindowIcon(QIcon(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"icon/logo_1024_1024.png").wstr())));
	setWindowFlags(Qt::Widget);

	QApplication* app = dynamic_cast<QApplication*>(QCoreApplication::instance());
	app->installEventFilter(new MouseReleaseFilter(this));

	refreshStyle();

	if (utility::getOsType() != OS_MAC)
	{
		// can only be done once, because resetting the style on the QCoreApplication causes crash
		app->setStyleSheet(
			utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"main/scrollbar.css")).c_str());
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

	for (DockWidget& dockWidget : m_dockWidgets)
	{
		dockWidget.toggle->clear();
	}
}

void QtMainWindow::addView(View* view)
{
	const QString name = QString::fromStdString(view->getName());
	if (name == "Tabs")
	{
		QToolBar* toolBar = new QToolBar();
		toolBar->setObjectName("Tool" + name);
		toolBar->setMovable(false);
		toolBar->setFloatable(false);
		toolBar->setStyleSheet("* { margin: 0; }");
		toolBar->addWidget(QtViewWidgetWrapper::getWidgetOfView(view));
		addToolBar(toolBar);
		return;
	}

	QDockWidget* dock = new QDockWidget(name, this);
	dock->setObjectName("Dock" + name);

	dock->setWidget(new QWidget());
	QVBoxLayout* layout = new QVBoxLayout(dock->widget());
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(QtViewWidgetWrapper::getWidgetOfView(view));

	// Disable un-intended vertical growth of search widget
	if (name == "Search")
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

	QAction* action = new QAction(name + " Window", this);
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

void QtMainWindow::overrideView(View* view)
{
	const QString name = QString::fromStdString(view->getName());
	if (name == "Tabs")
	{
		return;
	}

	QDockWidget* dock = nullptr;
	for (const DockWidget& dockWidget : m_dockWidgets)
	{
		if (dockWidget.widget->windowTitle() == name)
		{
			dock = dockWidget.widget;
			break;
		}
	}

	if (!dock)
	{
		LOG_ERROR_STREAM(<< "Couldn't find view to override: " << name.toStdString());
		return;
	}

	QWidget* oldWidget = dock->widget()->layout()->itemAt(0)->widget();
	QWidget* newWidget = QtViewWidgetWrapper::getWidgetOfView(view);

	if (oldWidget == newWidget)
	{
		return;
	}

	oldWidget = dock->widget()->layout()->takeAt(0)->widget();
	oldWidget->hide();
	dock->widget()->layout()->addWidget(newWidget);
	newWidget->show();
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
	QSettings settings(QString::fromStdWString(UserPaths::getWindowSettingsPath().wstr()), QSettings::IniFormat);

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
	QSettings settings(QString::fromStdWString(UserPaths::getWindowSettingsPath().wstr()), QSettings::IniFormat);
	this->restoreState(settings.value("DOCK_LOCATIONS").toByteArray());

	for (DockWidget dock : m_dockWidgets)
	{
		dock.action->setChecked(!dock.widget->isHidden());
	}
}

void QtMainWindow::loadWindow(bool showStartWindow, bool showEULA, bool enterLicense, const std::string& licenseError)
{
	if (showStartWindow)
	{
		showStartScreen();
	}

	if (enterLicense)
	{
		forceEnterLicense(licenseError);
	}

	if (showEULA)
	{
		showEula(true);
	}
}

void QtMainWindow::saveLayout()
{
	QSettings settings(QString::fromStdWString(UserPaths::getWindowSettingsPath().wstr()), QSettings::IniFormat);

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

void QtMainWindow::forceEnterLicense(const std::string& licenseError)
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

	if (licenseError.size())
	{
		window->setErrorMessage(QString::fromStdString(licenseError));
	}

	window->updateCloseButton("Quit");

	setEnabled(false);
	window->setEnabled(true);

	disconnect(window, &QtWindow::canceled, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtWindow::canceled, dynamic_cast<QApplication*>(QCoreApplication::instance()), &QApplication::quit);
}

void QtMainWindow::updateHistoryMenu(std::shared_ptr<MessageBase> message)
{
	const size_t historyMenuSize = 20;

	if (message && dynamic_cast<MessageActivateBase*>(message.get()))
	{
		std::vector<SearchMatch> matches = dynamic_cast<MessageActivateBase*>(message.get())->getSearchMatches();
		if (matches.size() && !matches[0].text.empty())
		{
			std::vector<std::shared_ptr<MessageBase>> history = { message };
			std::set<SearchMatch> uniqueMatches = { matches[0] };

			for (std::shared_ptr<MessageBase> m : m_history)
			{
				if (uniqueMatches.insert(dynamic_cast<MessageActivateBase*>(m.get())->getSearchMatches()[0]).second)
				{
					history.push_back(m);

					if (history.size() >= historyMenuSize)
					{
						break;
					}
				}
			}

			m_history = history;
		}
	}

	setupHistoryMenu();
}

void QtMainWindow::clearHistoryMenu()
{
	m_history.clear();
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
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"main/main.css")).c_str());

	QFont tooltipFont = QToolTip::font();
	tooltipFont.setPixelSize(ApplicationSettings::getInstance()->getFontSize());
	QToolTip::setFont(tooltipFont);
}

void QtMainWindow::setWindowsTaskbarProgress(float progress)
{
	m_windowsTaskbarButton.setProgress(progress);
}

void QtMainWindow::hideWindowsTaskbarProgress()
{
	m_windowsTaskbarButton.hideProgress();
}

void QtMainWindow::showEvent(QShowEvent* e)
{
	m_windowsTaskbarButton.setWindow(this);
}

void QtMainWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_Backspace:
			MessageHistoryUndo().dispatch();
			break;

		case Qt::Key_Escape:
			emit hideScreenSearch();
			emit hideIndexingDialog();
			break;

		case Qt::Key_Slash:
		case Qt::Key_Question:
			emit showScreenSearch();
			break;

		case Qt::Key_R:
			if (event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
			{
				MessageRefreshUI().dispatch();
			}
			break;

		case Qt::Key_F4:
			if (utility::getOsType() == OS_WINDOWS && event->modifiers() & Qt::ControlModifier)
			{
				closeTab();
			}
			break;

		case Qt::Key_Space:
			PRINT_TRACES();
			break;
	}
}

void QtMainWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu menu(event, this);
	menu.addUndoActions();
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
	MessageErrorsHelpMessage(true).dispatch();
}

void QtMainWindow::showChangelog()
{
	QDesktopServices::openUrl(QUrl("https://github.com/CoatiSoftware/SourcetrailBugTracker/#changelog"));
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
	ApplicationSettings::getInstance()->setAcceptedEulaVersion(Application::EULA_VERSION);
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

	MessageRefreshUI().dispatch();

	setEnabled(true);

	if (showStartWindow)
	{
		showStartScreen();
	}
}

void QtMainWindow::showDataFolder()
{
	QDesktopServices::openUrl(QUrl(QString::fromStdWString(L"file:///" + UserPaths::getUserDataPath().makeCanonical().wstr()), QUrl::TolerantMode));
}

void QtMainWindow::showLogFolder()
{
	QDesktopServices::openUrl(QUrl(QString::fromStdWString(L"file:///" + UserPaths::getLogPath().makeCanonical().wstr()), QUrl::TolerantMode));
}

void QtMainWindow::openTab()
{
	MessageTabOpen().dispatch();
}

void QtMainWindow::closeTab()
{
	MessageTabClose().dispatch();
}

void QtMainWindow::nextTab()
{
	MessageTabSelect(true).dispatch();
}

void QtMainWindow::previousTab()
{
	MessageTabSelect(false).dispatch();
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

void QtMainWindow::newProjectFromCDB(const FilePath& filePath)
{
	QtProjectWizzard* wizzard = dynamic_cast<QtProjectWizzard*>(m_windowStack.getTopWindow());
	if (!wizzard)
	{
		wizzard = createWindow<QtProjectWizzard>();
	}

	wizzard->newProjectFromCDB(filePath);
}

void QtMainWindow::openProject()
{
	QString fileName = QtFileDialog::getOpenFileName(
		this, tr("Open File"), FilePath(), "Sourcetrail Project Files (*.srctrlprj *.coatiproject)");

	if (!fileName.isEmpty())
	{
		MessageLoadProject(FilePath(fileName.toStdWString())).dispatch();
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
	MessageCodeReference(MessageCodeReference::REFERENCE_PREVIOUS, false).dispatch();
}

void QtMainWindow::codeReferenceNext()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_NEXT, false).dispatch();
}

void QtMainWindow::codeLocalReferencePrevious()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_PREVIOUS, true).dispatch();
}

void QtMainWindow::codeLocalReferenceNext()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_NEXT, true).dispatch();
}

void QtMainWindow::overview()
{
	MessageActivateAll().dispatch();
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
	MessageIndexingShowDialog().dispatch();
	MessageRefresh().dispatch();
}

void QtMainWindow::forceRefresh()
{
	MessageIndexingShowDialog().dispatch();
	MessageRefresh().refreshAll().dispatch();
}

void QtMainWindow::undo()
{
	MessageHistoryUndo().dispatch();
}

void QtMainWindow::redo()
{
	MessageHistoryRedo().dispatch();
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
		ResourcePaths::getFallbackPath().concatenate(L"window_settings.ini"),
		UserPaths::getWindowSettingsPath()
	);
	loadDockWidgetLayout();
}

void QtMainWindow::openRecentProject()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
	{
		MessageLoadProject(FilePath(action->data().toString().toStdWString())).dispatch();
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
			m_recentProjectAction[i]->setText(QString::fromStdWString(project.fileName()));
			m_recentProjectAction[i]->setData(QString::fromStdWString(project.wstr()));
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
	MessageBookmarkCreate().dispatch();
}

void QtMainWindow::showBookmarkBrowser()
{
	MessageBookmarkBrowse().dispatch();
}

void QtMainWindow::openHistoryAction()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		std::shared_ptr<MessageBase> m = m_history[action->data().toInt()];
		m->setSchedulerId(TabId::currentTab());
		m->setIsReplayed(false);
		m->dispatch();
	}
}

void QtMainWindow::activateBookmarkAction()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		std::shared_ptr<Bookmark> bookmark = m_bookmarks[action->data().toInt()];
		MessageBookmarkActivate(bookmark).dispatch();
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

	menu->addAction(tr("Next Reference"), this, &QtMainWindow::codeReferenceNext, QKeySequence(Qt::CTRL + Qt::Key_G));
	menu->addAction(tr("Previous Reference"), this,
		&QtMainWindow::codeReferencePrevious, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_G));

	menu->addAction(tr("Next Local Reference"), this,
		&QtMainWindow::codeLocalReferenceNext, QKeySequence(Qt::CTRL + Qt::Key_E));
	menu->addAction(tr("Previous Local Reference"), this,
		&QtMainWindow::codeLocalReferencePrevious, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_E));

	menu->addSeparator();

	menu->addAction(tr("&To overview"), this, &QtMainWindow::overview, QKeySequence::MoveToStartOfDocument);

	menu->addSeparator();

	menu->addAction(tr("Preferences..."), this, &QtMainWindow::openSettings, QKeySequence(Qt::CTRL + Qt::Key_Comma));
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("New Tab"), this, &QtMainWindow::openTab, QKeySequence::AddTab);
	menu->addAction(tr("Close Tab"), this, &QtMainWindow::closeTab, QKeySequence(Qt::CTRL + Qt::Key_W));

	if (utility::getOsType() == OS_MAC)
	{
		menu->addAction(tr("Select Next Tab"), this, &QtMainWindow::nextTab, QKeySequence(Qt::META + Qt::Key_Tab));
		menu->addAction(tr("Select Previous Tab"), this, &QtMainWindow::previousTab, QKeySequence(Qt::SHIFT + Qt::META + Qt::Key_Tab));
	}
	else
	{
		menu->addAction(tr("Select Next Tab"), this, &QtMainWindow::nextTab, QKeySequence(Qt::CTRL + Qt::Key_Tab));
		menu->addAction(tr("Select Previous Tab"), this, &QtMainWindow::previousTab, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_Tab));
	}

	menu->addSeparator();

	menu->addAction(tr("Show Start Window"), this, &QtMainWindow::showStartScreen);

	m_showTitleBarsAction = new QAction("Show Title Bars", this);
	m_showTitleBarsAction->setCheckable(true);
	m_showTitleBarsAction->setChecked(m_showDockWidgetTitleBars);
	connect(m_showTitleBarsAction, &QAction::triggered, this, &QtMainWindow::toggleShowDockWidgetTitleBars);
	menu->addAction(m_showTitleBarsAction);
	menu->addAction(tr("Reset Window Layout"), this, &QtMainWindow::resetWindowLayout);

	menu->addSeparator();

	m_viewSeparator = menu->addSeparator();

	menu->addAction(tr("Larger Font"), this, &QtMainWindow::zoomIn, QKeySequence::ZoomIn);
	menu->addAction(tr("Smaller Font"), this, &QtMainWindow::zoomOut, QKeySequence::ZoomOut);
	menu->addAction(tr("Reset Font Size"), this, &QtMainWindow::resetZoom, QKeySequence(Qt::CTRL + Qt::Key_0));

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
		MessageActivateBase* msg = dynamic_cast<MessageActivateBase*>(m_history[i].get());
		if (!msg)
		{
			continue;
		}

		const SearchMatch match = msg->getSearchMatches()[0];
		const std::wstring name = utility::elide(match.getFullName(), utility::ELIDE_RIGHT, 50);

		QAction* action = new QAction();
		action->setText(QString::fromStdWString(name));
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
		std::wstring name = utility::elide(bookmark->getName(), utility::ELIDE_RIGHT, 50);

		QAction* action = new QAction();
		action->setText(QString::fromStdWString(name));
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
	menu->addAction(tr("Changelog"), this, &QtMainWindow::showChangelog);
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
