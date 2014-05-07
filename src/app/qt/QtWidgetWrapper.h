#ifndef QT_WIDGET_WRAPPER_H
#define QT_WIDGET_WRAPPER_H

#include <memory>

#include <QtWidgets/qwidget.h>

#include "gui/WidgetWrapper.h"

class GuiElement;

class QtWidgetWrapper: public WidgetWrapper
{
public:
	static std::shared_ptr<QWidget> getWidgetOfElement(std::shared_ptr<GuiElement> element);
	static std::shared_ptr<QWidget> getWidgetOfElement(GuiElement* element);

	QtWidgetWrapper(std::shared_ptr<QWidget> widget);
	virtual ~QtWidgetWrapper();

	std::shared_ptr<QWidget> getWidget(); // not const because returned widget can be changed.

private:
	std::shared_ptr<QWidget> m_widget;
};

#endif // QT_WIDGET_WRAPPER_H
