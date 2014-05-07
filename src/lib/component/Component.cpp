#include "component/Component.h"

Component::Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller)
	: m_controller(controller)
	, m_view(view)
{
}

Component::~Component()
{
}
