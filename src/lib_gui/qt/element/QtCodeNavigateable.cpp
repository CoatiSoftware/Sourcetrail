#include "qt/element/QtCodeNavigateable.h"

#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>

#include "settings/ApplicationSettings.h"

QtCodeNavigateable::~QtCodeNavigateable()
{
}

void QtCodeNavigateable::ensureWidgetVisibleAnimated(
	QWidget* parentWidget, QWidget *childWidget, QRectF rect, bool animated, bool onTop)
{
	QAbstractScrollArea* area = getScrollArea();
	if (!area || !parentWidget->isAncestorOf(childWidget))
	{
		return;
	}

	const QRect microFocus = childWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
	const QRect defaultMicroFocus = childWidget->QWidget::inputMethodQuery(Qt::ImCursorRectangle).toRect();
	QRect focusRect = (microFocus != defaultMicroFocus)
		? QRect(childWidget->mapTo(parentWidget, microFocus.topLeft()), microFocus.size())
		: QRect(childWidget->mapTo(parentWidget, QPoint(0, 0)), childWidget->size());
	const QRect visibleRect(-parentWidget->pos(), area->viewport()->size());

	if (rect.height() > 0)
	{
		focusRect = QRect(childWidget->mapTo(parentWidget, rect.topLeft().toPoint()), rect.size().toSize());
		focusRect.adjust(0, 0, 0, 100);
	}

	QScrollBar* scrollBar = area->verticalScrollBar();
	int value = focusRect.center().y() - visibleRect.center().y();

	if (onTop)
	{
		value = focusRect.top() - visibleRect.top();
	}

	if (scrollBar && (value > 50 || value < -50))
	{
		if (animated && ApplicationSettings::getInstance()->getUseAnimations())
		{
			QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
			anim->setDuration(300);
			anim->setStartValue(scrollBar->value());
			anim->setEndValue(scrollBar->value() + value);
			anim->setEasingCurve(QEasingCurve::InOutQuad);
			anim->start();
		}
		else
		{
			scrollBar->setValue(scrollBar->value() + value);
		}
	}
}

void QtCodeNavigateable::ensurePercentVisibleAnimated(double percentA, double percentB, bool animated, bool onTop)
{
	QAbstractScrollArea* area = getScrollArea();
	if (!area)
	{
		return;
	}

	int totalHeight = area->sizeHint().height();
	int visibleHeight = area->viewport()->height();
	int scrollableHeight = totalHeight - visibleHeight;
	if (scrollableHeight < 0)
	{
		return;
	}

	int scrollHeight = totalHeight * percentA;
	if (!onTop)
	{
		if (percentB)
		{
			int scrollHeightB = totalHeight * percentB;
			int rectHeight = scrollHeightB - scrollHeight;

			if (rectHeight < visibleHeight)
			{
				if (rectHeight < visibleHeight / 2)
				{
					scrollHeight -= visibleHeight / 4;
				}
				else
				{
					scrollHeight += rectHeight / 2 - visibleHeight / 2;
				}
			}
			else
			{
				scrollHeight -= 20;
			}
		}
		else
		{
			scrollHeight -= visibleHeight / 4;
		}
	}
	else
	{
		scrollHeight -= 20;
	}

	QScrollBar* scrollBar = area->verticalScrollBar();
	double scrollFactor = double(scrollBar->maximum()) / scrollableHeight;

	int value = scrollHeight * scrollFactor;

	if (animated && ApplicationSettings::getInstance()->getUseAnimations())
	{
		QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
		anim->setDuration(300);
		anim->setStartValue(scrollBar->value());
		anim->setEndValue(value);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
	else
	{
		scrollBar->setValue(value);
	}
}
