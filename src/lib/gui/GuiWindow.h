#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <memory>

class GuiCanvas;

class GuiWindow
{
public:
	GuiWindow();
	virtual ~GuiWindow();

	virtual void setCanvas(std::shared_ptr<GuiCanvas> canvas) = 0;
};

#endif // GUI_WINDOW_H
