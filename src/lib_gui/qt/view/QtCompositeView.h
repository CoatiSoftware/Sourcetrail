#ifndef QT_COMPOSITE_VIEW
#define QT_COMPOSITE_VIEW

#include "CompositeView.h"
#include "QtThreadedFunctor.h"

class QBoxLayout;
class QWidget;

class QtCompositeView: public CompositeView
{
public:
	QtCompositeView(
		ViewLayout* viewLayout, CompositeDirection direction, const std::string& name, Id tabId);
	~QtCompositeView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// CompositeView implementation
	void addViewWidget(View* view) override;
	void showFocusIndicator(bool focus) override;

private:
	QtThreadedLambdaFunctor m_onQtThread;
	QWidget* m_focusIndicator;
	QWidget* m_widget;
	QBoxLayout* m_layout;
};

#endif	  // QT_COMPOSITE_VIEW
