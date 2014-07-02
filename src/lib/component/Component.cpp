#include "component/Component.h"

#include "component/view/View.h"
#include "component/controller/Controller.h"

Component::Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller)
	: m_controller(controller)
	, m_view(view)
{
	m_controller->setComponent(this);
	m_view->setComponent(this);
}

Component::~Component()
{
	m_controller->setComponent(NULL);
	m_view->setComponent(NULL);
}
