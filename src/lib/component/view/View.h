#ifndef VIEW_H
#define VIEW_H

#include <memory>
#include <string>

#include "component/Component.h"
#include "component/view/ViewLayout.h"
#include "utility/math/Vector2.h"

class ViewWidgetWrapper;

class View
{
public:
	template<typename T>
	static std::shared_ptr<T> createInitAndAddToLayout(ViewLayout* viewLayout);

	template<typename T>
	static std::shared_ptr<T> createAndInit(ViewLayout* viewLayout);

	View(ViewLayout* viewLayout);
	virtual ~View();

	virtual std::string getName() const = 0;

	virtual void createWidgetWrapper() = 0;
	virtual void initView() = 0;
	virtual void refreshView() = 0;

	void init();
	void addToLayout();

	void setComponent(Component* component);

	ViewWidgetWrapper* getWidgetWrapper() const;

	void createOverlay();
	void showOverlay();
	void hideOverlay();

protected:
	template <typename ControllerType>
	ControllerType* getController();

	ViewLayout* getViewLayout() const;

	void setWidgetWrapper(std::shared_ptr<ViewWidgetWrapper> widgetWrapper);

private:
	Component* m_component;
	ViewLayout* const m_viewLayout;
	std::shared_ptr<ViewWidgetWrapper> m_widgetWrapper;
};

template<typename T>
std::shared_ptr<T> View::createInitAndAddToLayout(ViewLayout* viewLayout)
{
	std::shared_ptr<T> ptr = View::createAndInit<T>(viewLayout);

	ptr->addToLayout();

	return ptr;
}

template<typename T>
std::shared_ptr<T> View::createAndInit(ViewLayout* viewLayout)
{
	std::shared_ptr<T> ptr = std::make_shared<T>(viewLayout);

	ptr->init();

	return ptr;
}

template <typename ControllerType>
ControllerType* View::getController()
{
	if (m_component)
	{
		return m_component->getController<ControllerType>();
	}
	return NULL;
}

#endif // VIEW_H
