#include "component/view/View.h"

#include "component/view/ViewLayout.h"
#include "gui/GuiWidgetWrapper.h"

View::~View()
{
	m_viewLayout->removeView(this);
}

void View::setWidgetWrapper(std::shared_ptr<GuiWidgetWrapper> widgetWrapper)
{
	m_widgetWrapper = widgetWrapper;
}

GuiWidgetWrapper* View::getWidgetWrapper()
{
	return m_widgetWrapper.get();
}

void View::setComponent(Component* component)
{
	m_component = component;
}

int View::getMinWidth() const
{
	return m_minSize.x;
}

int View::getMinHeight() const
{
	return m_minSize.y;
}

Vec2i View::getMinSize() const
{
	return m_minSize;
}

View::View(ViewLayout* viewLayout, const Vec2i& minSize)
	: m_viewLayout(viewLayout)
	, m_widgetWrapper(nullptr)
	, m_minSize(minSize)
{
}
