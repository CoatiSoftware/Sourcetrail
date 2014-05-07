#include "gui/GuiContainer.h"

GuiContainer::GuiContainer(std::shared_ptr<WidgetWrapper> widgetWrapper)
	: GuiElement(widgetWrapper)
{
}

GuiContainer::~GuiContainer()
{
}
