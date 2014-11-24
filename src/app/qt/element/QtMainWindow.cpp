#include "qt/element/QtMainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include "component/view/View.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageFind.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageLoadSource.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSaveProject.h"

QtMainWindow::QtMainWindow()
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
	setDockNestingEnabled(true);

	setupProjectMenu();
	setupViewMenu();
	setupFindMenu();
	setupHelpMenu();
}

QtMainWindow::~QtMainWindow()
{
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
		)
	);
}

void QtMainWindow::newProject()
{
	QString sourceDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

	if (!sourceDir.isEmpty())
	{
		MessageLoadSource(sourceDir.toStdString()).dispatch();
	}
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

void QtMainWindow::addView(View* view)
{
	QDockWidget* dock = new QDockWidget(tr(view->getName().c_str()), this);
	dock->setWidget(QtViewWidgetWrapper::getWidgetOfView(view));
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

	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		View* view = m_dockWidgets[i].first;
		QDockWidget* dockWidget = m_dockWidgets[i].second;

		settings.beginGroup(view->getName().c_str());
		dockWidget->setFloating(settings.value("floating", false).toBool());
		dockWidget->setHidden(settings.value("hidden", false).toBool());
		dockWidget->resize(settings.value("size", QSize(400, 400)).toSize());
		dockWidget->move(settings.value("position", QPoint(200, 200)).toPoint());
		settings.endGroup();
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

	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		View* view = m_dockWidgets[i].first;
		QDockWidget* dockWidget = m_dockWidgets[i].second;

		settings.beginGroup(view->getName().c_str());
		settings.setValue("floating", dockWidget->isFloating());
		settings.setValue("hidden", dockWidget->isHidden());
		settings.setValue("size", dockWidget->size());
		settings.setValue("position", dockWidget->pos());
		settings.endGroup();
	}
}

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&New"), this, SLOT(newProject()), QKeySequence::New);
	menu->addAction(tr("&Open..."), this, SLOT(openProject()), QKeySequence::Open);
	menu->addAction(tr("&Save"), this, SLOT(saveProject()), QKeySequence::Save);
	menu->addAction(tr("Save as"), this, SLOT(saveAsProject()), QKeySequence::SaveAs);
	menu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::setupViewMenu()
{
	QMenu *menu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&Close Window"), this, SLOT(closeWindow()), QKeySequence::Close);
	menu->addAction(tr("&Refresh"), this, SLOT(refresh()), QKeySequence::Refresh);
}

void QtMainWindow::setupFindMenu()
{
	QMenu *menu = new QMenu(tr("&Find"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&Find"), this, SLOT(find()), QKeySequence::Find);
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("About &Qt"), QCoreApplication::instance(), SLOT(aboutQt()));
}

QDockWidget* QtMainWindow::getDockWidgetForView(View* view) const
{
	for (size_t i = 0; i < m_dockWidgets.size(); i++)
	{
		if (m_dockWidgets[i].first == view)
		{
			return m_dockWidgets[i].second;
		}
	}

	LOG_ERROR("DockWidget was not found for view.");
	return nullptr;
}
