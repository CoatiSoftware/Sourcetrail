#include "component/Component.h"

Component::Component(const std::shared_ptr<Controller>& controller, const std::shared_ptr<View>& view)
: m_controller(controller)
, m_view(view)
{
}

Component::~Component()
{
}
