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
	static std::shared_ptr<T> create(ViewLayout* viewLayout);

	View(ViewLayout* viewLayout, const Vec2i& minSize);
	virtual ~View();

	virtual std::string getName() const = 0;
	virtual void createWidgetWrapper() = 0;
	virtual void initView() = 0;
	virtual void refreshView() = 0;

	void setComponent(Component* component);

	void setWidgetWrapper(std::shared_ptr<ViewWidgetWrapper> widgetWrapper);
	ViewWidgetWrapper* getWidgetWrapper() const;

	int getMinWidth() const;
	int getMinHeight() const;
	Vec2i getMinSize() const;

protected:
	template <typename ControllerType>
	ControllerType* getController();

	ViewLayout* getViewLayout() const;

private:
	Component* m_component;
	ViewLayout* const m_viewLayout;
	std::shared_ptr<ViewWidgetWrapper> m_widgetWrapper;
	Vec2i m_minSize;
};

template<typename T>
std::shared_ptr<T> View::create(ViewLayout* viewLayout)
{
	std::shared_ptr<T> ptr = std::make_shared<T>(viewLayout);

	ptr->createWidgetWrapper();
	ptr->initView();

	viewLayout->addView(ptr.get());

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
