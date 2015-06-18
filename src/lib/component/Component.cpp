#include "component/Component.h"

#include "component/view/View.h"
#include "component/controller/Controller.h"

Component::Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller)
	: m_controller(controller)
	, m_view(view)
{
	if (m_controller)
	{
		m_controller->setComponent(this);
	}

	if (m_view)
	{
		m_view->setComponent(this);
	}
}

Component::~Component()
{
	if (m_controller)
	{
		m_controller->setComponent(NULL);
	}

	if (m_view)
	{
		m_view->setComponent(NULL);
	}
}
