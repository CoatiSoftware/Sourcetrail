#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

class View;
class Controller;

class Component
{
public:
	Component(std::shared_ptr<View> view, std::shared_ptr<Controller> controller);
	~Component();

	template <typename ControllerType>
	ControllerType* getController() const;

	Controller* getControllerPtr() const;

	template <typename ViewType>
	ViewType* getView() const;

	View* getViewPtr() const;

private:
	const std::shared_ptr<Controller> m_controller;
	const std::shared_ptr<View> m_view;
};


template <typename ControllerType>
ControllerType* Component::getController() const
{
	return dynamic_cast<ControllerType*>(m_controller.get());
}

template <typename ViewType>
ViewType* Component::getView() const
{
	return dynamic_cast<ViewType*>(m_view.get());
}

#endif // COMPONENT_H
