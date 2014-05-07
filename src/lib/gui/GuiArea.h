#ifndef GUI_AREA_H
#define GUI_AREA_H

#include "gui/GuiControl.h"

class GuiArea: public GuiControl
{
public:
	GuiArea(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiArea();

};

#endif // GUI_AREA_H
