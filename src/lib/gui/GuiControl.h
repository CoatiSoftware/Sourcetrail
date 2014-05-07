#ifndef GUI_CONTROL_H
#define GUI_CONTROL_H

#include "gui/GuiElement.h"

class GuiControl: public GuiElement
{
public:
	GuiControl(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiControl();

};

#endif // GUI_CONTROL_H
