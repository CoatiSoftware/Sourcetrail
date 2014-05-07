#ifndef GUI_CANVAS_H
#define GUI_CANVAS_H

#include "gui/GuiContainer.h"

class GuiCanvas: public GuiContainer
{
public:
	GuiCanvas(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiCanvas();
};

#endif // GUI_CANVAS_H
