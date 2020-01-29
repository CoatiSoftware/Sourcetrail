#include "QtHoverButton.h"

QtHoverButton::QtHoverButton(QWidget* parent): QPushButton(QLatin1String(""), parent)
{
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	setMouseTracking(true);
}

void QtHoverButton::enterEvent(QEvent* event)
{
	if (isEnabled())
	{
		emit hoveredIn(this);
	}
}

void QtHoverButton::leaveEvent(QEvent* event)
{
	if (isEnabled())
	{
		emit hoveredOut(this);
	}
}
