#ifndef QT_TABBED_VIEW
#define QT_TABBED_VIEW

#include <QObject>

#include "../utility/QtThreadedFunctor.h"
#include "../../../lib/component/view/TabbedView.h"

class QTabWidget;
class QtSelfRefreshIconButton;

class QtTabbedView
	: public QObject
	, public TabbedView
{
public:
	QtTabbedView(ViewLayout* viewLayout, const std::string& name);
	~QtTabbedView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// TabbedView implementation
	void addViewWidget(View* view) override;
	void showView(View* view) override;

private:
	void setStyleSheet();
	bool eventFilter(QObject* obj, QEvent* event) override;

	QtThreadedLambdaFunctor m_onQtThread;
	QTabWidget* m_widget;
	QtSelfRefreshIconButton* m_closeButton;
};

#endif	  // QT_TABBED_VIEW
