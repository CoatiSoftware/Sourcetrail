#include "qt/element/QtMainWindow.h"

#include <QDockWidget>

#include "component/view/View.h"
#include "qt/QtWidgetWrapper.h"

QtMainWindow::QtMainWindow()
{
	setObjectName("QtMainWindow");
	setCentralWidget(nullptr);
}

QtMainWindow::~QtMainWindow()
{
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
