#include "gui/GuiElement.h"

#include "gui/WidgetWrapper.h"

GuiElement::GuiElement(std::shared_ptr<WidgetWrapper> widgetWrapper)
	: m_widgetWrapper(widgetWrapper)
{
}

GuiElement::~GuiElement()
{
}

void GuiElement::setBackgroundColor(int r, int g, int b, int a)
{
	setBackgroundColor(Colori(r, g, b, a));
}

std::shared_ptr<WidgetWrapper> GuiElement::getWidgetWrapper()
{
	return m_widgetWrapper;
}
