#include "qt/element/QtMainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include "version.h"

#include "component/view/View.h"
#include "component/view/CompositeView.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageInterruptTasks.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageLoadSource.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSaveProject.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "utility/messaging/type/MessageZoom.h"

QtMainWindow::QtMainWindow()
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
	// StartScreen
	m_startScreen = std::make_shared<QtStartScreen>(this);
	m_startScreen->setup();
	m_startScreen->show();
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
	m_dockWidgets.push_back(std::make_pair(view, dock));
}

void QtMainWindow::removeView(View* view)
{
	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		if (m_dockWidgets[i].first == view)
		{
			removeDockWidget(m_dockWidgets[i].second);
			m_dockWidgets.erase(m_dockWidgets.begin() + i);
			return;
		}
	}
}

void QtMainWindow::showView(View* view)
{
	getDockWidgetForView(view)->setHidden(false);
}

void QtMainWindow::hideView(View* view)
{
	getDockWidgetForView(view)->setHidden(true);
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
	QMessageBox::about(
		this,
		tr("About"),
		tr(
			"Developed by:\n\n"
			"Manuel Dobusch\n"
			"Eberhard Gräther\n"
			"Malte Langkabel\n"
			"Victoria Pfausler\n"
			"Andreas Stallinger\n"
			"\nVersion: " GIT_VERSION_NUMBER
			"\nBranch: " GIT_BRANCH
			"\nBuildtype: " BUILD_TYPE
			"\nDate: " GIT_COMMIT_TIME
		)
	);
}

void QtMainWindow::newProject()
{
	m_newProjectDialog = std::make_shared<QtProjectSetupScreen>(this);
	m_newProjectDialog->setup();
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
	}
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

void QtMainWindow::handleEscapeShortcut()
{
	if(m_startScreen)
	{
		m_startScreen->hide();
	}
	if(m_newProjectDialog)
	{
		m_newProjectDialog->hide();
	}
	MessageInterruptTasks().dispatch();
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Ope&n Source Folder..."), this, SLOT(newProject()), QKeySequence::New);
	menu->addAction(tr("&Open Coati Project..."), this, SLOT(openProject()), QKeySequence::Open);
	menu->addAction(tr("&Save"), this, SLOT(saveProject()), QKeySequence::Save);
	menu->addAction(tr("Save as..."), this, SLOT(saveAsProject()), QKeySequence::SaveAs);
	menu->addAction(tr("&Close Window"), this, SLOT(closeWindow()), QKeySequence::Close);
	menu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::showLicences()
{
	//TODO: Licences
	//QMessageBox::information()
}

void QtMainWindow::setupEditMenu()
{
	QMenu *menu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&Refresh"), this, SLOT(refresh()), QKeySequence::Refresh);
	menu->addAction(tr("Undo"), this, SLOT(undo()), QKeySequence::Undo);
	menu->addAction(tr("Redo"), this, SLOT(redo()), QKeySequence::Redo);
	menu->addAction(tr("&Find"), this, SLOT(find()), QKeySequence::Find);
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("Larger font"), this, SLOT(zoomIn()), QKeySequence::ZoomIn);
	menu->addAction(tr("Smaller font"), this, SLOT(zoomOut()), QKeySequence::ZoomOut);

	menu->addAction(tr("Switch Color Scheme..."), this, SLOT(switchColorScheme()));
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("About &Qt"), QCoreApplication::instance(), SLOT(aboutQt()));
	//menu->addAction(tr("Licences"), QCoreApplication::instance(), SLOT(showLicences()));
}

void QtMainWindow::setupShortcuts()
{
	m_escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(m_escapeShortcut, SIGNAL(activated()), SLOT(handleEscapeShortcut()));
}

QDockWidget* QtMainWindow::getDockWidgetForView(View* view) const
{
	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		CompositeView* compositeView = dynamic_cast<CompositeView*>(m_dockWidgets[i].first);
		if (compositeView)
		{
			for (View* v : compositeView->getViews())
			{
				if (v == view)
				{
					return m_dockWidgets[i].second;
				}
			}
		}
		else if (m_dockWidgets[i].first == view)
		{
			return m_dockWidgets[i].second;
		}
	}

	LOG_ERROR("DockWidget was not found for view.");
	return nullptr;
}
