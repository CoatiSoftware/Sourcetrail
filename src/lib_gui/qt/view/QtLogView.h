#ifndef QT_LOG_VIEW_H
#define QT_LOG_VIEW_H

#include "component/view/LogView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtLogView
	: public LogView
{
public:
	QtLogView(ViewLayout* viewLayout);
	virtual ~QtLogView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void clear();

private:
	void doClear();
	void doRefreshView();

	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
};

#endif // QT_LOG_VIEW_H
