#include "qt/QtWindow.h"

#include "gui/GuiCanvas.h"
#include "QtWidgetWrapper.h"

QtWindow::QtWindow()
{
	m_window.setObjectName("QtWindow");
	m_window.show();
}

QtWindow::~QtWindow()
{
}

void QtWindow::setCanvas(std::shared_ptr<GuiCanvas> canvas)
{
	m_window.setCentralWidget(QtWidgetWrapper::getWidgetOfElement(canvas).get());
}
