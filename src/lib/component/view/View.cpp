#include "component/view/View.h"

#include "component/view/ViewWidgetWrapper.h"

View::View(ViewLayout* viewLayout)
	: m_viewLayout(viewLayout)
	, m_widgetWrapper(nullptr)
{
}

View::~View()
{
	m_viewLayout->removeView(this);
}

void View::init()
{
	createWidgetWrapper();

	initView();
}

void View::addToLayout()
{
	m_viewLayout->addView(this);
}

void View::setComponent(Component* component)
{
	m_component = component;
}

ViewWidgetWrapper* View::getWidgetWrapper() const
{
	return m_widgetWrapper.get();
}

void View::createOverlay()
{
	getWidgetWrapper()->createOverlay();
}

void View::showOverlay()
{
	getWidgetWrapper()->showOverlay();
}

void View::hideOverlay()
{
	getWidgetWrapper()->hideOverlay();
}

ViewLayout* View::getViewLayout() const
{
	return m_viewLayout;
}

void View::setWidgetWrapper(std::shared_ptr<ViewWidgetWrapper> widgetWrapper)
{
	m_widgetWrapper = widgetWrapper;
}
