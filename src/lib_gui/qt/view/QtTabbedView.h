#ifndef QT_TABBED_VIEW
#define QT_TABBED_VIEW

#include <QObject>

#include "TabbedView.h"
#include "QtThreadedFunctor.h"

class QTabWidget;
class QtSelfRefreshIconButton;

class QtTabbedView
	: public QObject
	, public TabbedView
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
	virtual void showView(View* view);

private:
	void setStyleSheet();
	bool eventFilter(QObject* obj, QEvent* event);

	QtThreadedLambdaFunctor m_onQtThread;
	QTabWidget* m_widget;
	QtSelfRefreshIconButton* m_closeButton;
};

#endif // QT_TABBED_VIEW
