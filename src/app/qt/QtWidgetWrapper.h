#ifndef QT_WIDGET_WRAPPER_H
#define QT_WIDGET_WRAPPER_H

#include <memory>

#include <QtWidgets/qwidget.h>

#include "gui/GuiWidgetWrapper.h"

class View;

class QtWidgetWrapper: public GuiWidgetWrapper
{
public:
	static QWidget* getWidgetOfView(View* view);

	QtWidgetWrapper(std::shared_ptr<QWidget> widget);
	virtual ~QtWidgetWrapper();

	QWidget* getWidget();

private:
	std::shared_ptr<QWidget> m_widget;
};

#endif // QT_WIDGET_WRAPPER_H
