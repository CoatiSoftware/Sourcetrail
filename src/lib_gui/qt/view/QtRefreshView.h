#ifndef QT_REFRESH_VIEW_H
#define QT_REFRESH_VIEW_H

#include "component/view/RefreshView.h"
#include "qt/element/QtRefreshBar.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtRefreshView
	: public RefreshView
{
public:
	QtRefreshView(ViewLayout* viewLayout);
	~QtRefreshView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// RefreshView implementation

private:
	void setStyleSheet();

	QtThreadedLambdaFunctor m_onQtThread;

	QtRefreshBar* m_widget;
};

# endif // QT_REFRESH_VIEW_H
