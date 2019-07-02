#include "View.h"

#include "ViewWidgetWrapper.h"

View::View(ViewLayout* viewLayout)
	: m_component(nullptr)
	, m_viewLayout(viewLayout)
	, m_widgetWrapper(nullptr)
{
}

void View::addToLayout()
{
	m_viewLayout->addView(this);
}

void View::showDockWidget()
{
	m_viewLayout->showView(this);
}

ViewWidgetWrapper* View::getWidgetWrapper() const
{
	return m_widgetWrapper.get();
}

void View::setComponent(Component* component)
{
	m_component = component;
}

ViewLayout* View::getViewLayout() const
{
	return m_viewLayout;
}

void View::setEnabled(bool enabled)
{
	return getViewLayout()->setViewEnabled(this, enabled);
}

void View::setWidgetWrapper(std::shared_ptr<ViewWidgetWrapper> widgetWrapper)
{
	m_widgetWrapper = widgetWrapper;
}
