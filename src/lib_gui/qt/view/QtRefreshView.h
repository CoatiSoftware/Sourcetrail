#ifndef QT_REFRESH_VIEW_H
#define QT_REFRESH_VIEW_H

#include "QtThreadedFunctor.h"
#include "RefreshView.h"

class QFrame;

class QtRefreshView: public RefreshView
{
public:
	QtRefreshView(ViewLayout* viewLayout);
	~QtRefreshView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QFrame* m_widget;
};

#endif	  // QT_REFRESH_VIEW_H
