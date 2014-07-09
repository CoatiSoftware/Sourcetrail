#include "qt/element/QtMainWindow.h"

#include <iostream>

#include <QCoreApplication>
#include <QFileDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>

#include "component/view/View.h"
#include "qt/QtWidgetWrapper.h"
#include "utility/messaging/type/MessageLoadProject.h"

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

void QtMainWindow::openProject(const QString &path)
{
	QString fileName = path;

	if (fileName.isNull())
	{
		fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "XML Files (*.xml)");
	}

	if (!fileName.isEmpty())
	{
		std::cout << fileName.toStdString() << std::endl;
		MessageLoadProject message(fileName.toStdString());
		message.dispatch();
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
	QMenu *fileMenu = new QMenu(tr("&Project"), this);
	menuBar()->addMenu(fileMenu);

	fileMenu->addAction(tr("&Open..."), this, SLOT(openProject()), QKeySequence::Open);
	fileMenu->addAction(tr("E&xit"), QCoreApplication::instance(), SLOT(quit()), QKeySequence::Quit);
}

void QtMainWindow::setupHelpMenu()
{
	QMenu *helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);

	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	helpMenu->addAction(tr("About &Qt"), QCoreApplication::instance(), SLOT(aboutQt()));
}
