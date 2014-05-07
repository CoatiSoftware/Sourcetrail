#ifndef QT_WINDOW_H
#define QT_WINDOW_H

#include <memory>

#include <QtWidgets/QMainWindow>

#include "gui/GuiWindow.h"

class GuiCanvas;

class QtWindow: public GuiWindow
{
public:
	QtWindow();
	~QtWindow();

	void setCanvas(std::shared_ptr<GuiCanvas> canvas);

private:
	QMainWindow m_window;
};

# endif // QT_WINDOW_H
