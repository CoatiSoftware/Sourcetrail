#ifndef QT_TOOLTIP_VIEW
#define QT_TOOLTIP_VIEW

#include "TooltipView.h"
#include "QtThreadedFunctor.h"

class QTimer;
class QtTooltip;

class QtTooltipView
	: public TooltipView
{
public:
	QtTooltipView(ViewLayout* viewLayout);
	~QtTooltipView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void showTooltip(TooltipInfo info, const View* parent);
	virtual void hideTooltip(bool force);

	virtual bool tooltipVisible() const;

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QtTooltip* m_widget;
};

#endif // QT_TOOLTIP_VIEW
