#ifndef QT_VIEW_WIDGET_WRAPPER_H
#define QT_VIEW_WIDGET_WRAPPER_H

#include <QWidget>

#include "component/view/ViewWidgetWrapper.h"

class View;
class QtViewOverlay;

class QtViewWidgetWrapper
	: public ViewWidgetWrapper
{
public:
	static QWidget* getWidgetOfView(const View* view);

	QtViewWidgetWrapper(QWidget* widget);
	virtual ~QtViewWidgetWrapper();

	virtual void createOverlay() override;
	virtual void showOverlay() override;
	virtual void hideOverlay() override;

	QWidget* getWidget();

private:
	QWidget* m_widget;

	QtViewOverlay* m_overlay;
};

#endif // QT_VIEW_WIDGET_WRAPPER_H
