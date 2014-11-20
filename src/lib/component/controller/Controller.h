#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "component/Component.h"

class Controller
{
public:
	Controller();
	virtual ~Controller();

	void setComponent(Component* component);

protected:
	template <typename ViewType>
	ViewType* getView() const;

private:
	Component* m_component;

};


template <typename ViewType>
ViewType* Controller::getView() const
{
	if (m_component)
		return m_component->getView<ViewType>();
	return NULL;
}

#endif // CONTROLLER_H
