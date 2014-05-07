#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

#include "component/controller/Controller.h"
#include "component/view/View.h"

class Component
{
public:
	Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller);

	~Component();

private:
	std::shared_ptr<Controller> m_controller;
	std::shared_ptr<View> m_view;
};


#endif // COMPONENT_H
