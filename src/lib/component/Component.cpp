#include "component/Component.h"

Component::Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller)
	: m_controller(controller)
	, m_view(view)
{
}

Component::~Component()
{
}

Controller* Component::getController() const
{
	return m_controller.get();
}

View* Component::getView() const
{
	return m_view.get();
}
