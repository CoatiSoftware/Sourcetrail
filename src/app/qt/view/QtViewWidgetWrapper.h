#ifndef QT_VIEW_WIDGET_WRAPPER_H
#define QT_VIEW_WIDGET_WRAPPER_H

#include <QWidget>

#include "component/view/ViewWidgetWrapper.h"

class View;

class QtViewWidgetWrapper: public ViewWidgetWrapper
{
public:
	static QWidget* getWidgetOfView(const View* view);

	QtViewWidgetWrapper(QWidget* widget);
	virtual ~QtViewWidgetWrapper();

	QWidget* getWidget();

private:
	QWidget* m_widget;
};

#endif // QT_VIEW_WIDGET_WRAPPER_H
