#ifndef QT_VIEW_WIDGET_WRAPPER_H
#define QT_VIEW_WIDGET_WRAPPER_H

#include <memory>

#include <QWidget>

#include "component/view/ViewWidgetWrapper.h"

class View;

class QtViewWidgetWrapper: public ViewWidgetWrapper
{
public:
	static QWidget* getWidgetOfView(const View* view);

	QtViewWidgetWrapper(std::shared_ptr<QWidget> widget);
	virtual ~QtViewWidgetWrapper();

	QWidget* getWidget();

private:
	std::shared_ptr<QWidget> m_widget;
};

#endif // QT_VIEW_WIDGET_WRAPPER_H
