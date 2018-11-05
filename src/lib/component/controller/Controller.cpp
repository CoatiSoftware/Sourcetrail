#include  "component/controller/Controller.h"

Controller::Controller()
	: m_component(nullptr)
{
}

Controller::~Controller()
{
}

void Controller::setComponent(Component* component)
{
	m_component = component;
}

Id Controller::getTabId() const
{
	if (m_component)
	{
		return m_component->getTabId();
	}

	return 0;
}
