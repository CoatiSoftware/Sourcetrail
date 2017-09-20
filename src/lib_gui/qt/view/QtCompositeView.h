#ifndef QT_COMPOSITE_VIEW
#define QT_COMPOSITE_VIEW

#include <QWidget>

#include "component/view/CompositeView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtCompositeView
	: public CompositeView
{
public:
	QtCompositeView(ViewLayout* viewLayout, CompositeDirection direction, const std::string& name);
	~QtCompositeView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// CompositeView implementation
	virtual void addViewWidget(View* view);

private:
	QtThreadedLambdaFunctor m_onQtThread;
	QWidget* m_widget;
};

#endif // QT_COMPOSITE_VIEW
