#ifndef QT_TABBED_VIEW
#define QT_TABBED_VIEW

#include "component/view/TabbedView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QTabWidget;

class QtTabbedView
	: public TabbedView
{
public:
	QtTabbedView(ViewLayout* viewLayout, const std::string& name);
	~QtTabbedView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// TabbedView implementation
	virtual void addViewWidget(View* view);

private:
	void setStyleSheet();
	void doRefreshView();

	QtThreadedFunctor<void> m_refreshFunctor;
	QTabWidget* m_widget;
};

#endif // QT_TABBED_VIEW
