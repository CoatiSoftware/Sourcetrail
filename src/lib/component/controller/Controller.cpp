#include  "component/controller/Controller.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::setComponent(Component* component)
{
	m_component = component;
}
