#ifndef QT_COMPOSITE_VIEW
#define QT_COMPOSITE_VIEW

#include <QWidget>

#include "CompositeView.h"
#include "QtThreadedFunctor.h"

class QtCompositeView: public CompositeView
{
public:
	QtCompositeView(ViewLayout* viewLayout, CompositeDirection direction, const std::string& name);
	~QtCompositeView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// CompositeView implementation
	void addViewWidget(View* view) override;

private:
	QtThreadedLambdaFunctor m_onQtThread;
	QWidget* m_widget;
};

#endif	  // QT_COMPOSITE_VIEW
