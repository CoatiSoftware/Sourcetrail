#include "component/view/View.h"

#include "component/view/ViewWidgetWrapper.h"

View::View(ViewLayout* viewLayout, const Vec2i& minSize)
	: m_viewLayout(viewLayout)
	, m_widgetWrapper(nullptr)
	, m_minSize(minSize)
{
}

View::~View()
{
	m_viewLayout->removeView(this);
}

void View::setWidgetWrapper(std::shared_ptr<ViewWidgetWrapper> widgetWrapper)
{
	m_widgetWrapper = widgetWrapper;
}

ViewWidgetWrapper* View::getWidgetWrapper() const
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

ViewLayout* View::getViewLayout() const
{
	return m_viewLayout;
}
