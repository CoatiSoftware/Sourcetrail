#include "gui/GuiControl.h"

GuiControl::GuiControl(std::shared_ptr<WidgetWrapper> widgetWrapper)
	: GuiElement(widgetWrapper)
{
}

GuiControl::~GuiControl()
{
}
