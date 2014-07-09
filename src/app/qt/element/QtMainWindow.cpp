#include "qt/element/QtMainWindow.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>

#include "component/view/View.h"
#include "qt/QtWidgetWrapper.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageLoadSource.h"

QtMainWindow::QtMainWindow()
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);

	setupProjectMenu();
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

void QtMainWindow::addView(View* view)
{
	QDockWidget* dock = new QDockWidget(tr(view->getName().c_str()), this);
	dock->setWidget(QtWidgetWrapper::getWidgetOfView(view));
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

void QtMainWindow::setupProjectMenu()
{
	QMenu *menu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&New"), this, SLOT(newProject()), QKeySequence::New);
	menu->addAction(tr("&Open..."), this, SLOT(openProject()), QKeySequence::Open);
	menu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *menu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(menu);

	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("About &Qt"), QCoreApplication::instance(), SLOT(aboutQt()));
}
