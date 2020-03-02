// from: http://doc.qt.io/qt-5/qtwidgets-layouts-flowlayout-flowlayout-cpp.html

#include "QtFlowLayout.h"

#include <QWidget>

QtFlowLayout::QtFlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
	: QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}

QtFlowLayout::QtFlowLayout(int margin, int hSpacing, int vSpacing)
	: m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}

QtFlowLayout::~QtFlowLayout()
{
	QLayoutItem* item;
	while ((item = takeAt(0)))
	{
		delete item;
	}
}

void QtFlowLayout::addItem(QLayoutItem* item)
{
	itemList.append(item);
}

int QtFlowLayout::horizontalSpacing() const
{
	if (m_hSpace >= 0)
	{
		return m_hSpace;
	}
	else
	{
		return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
	}
}

int QtFlowLayout::verticalSpacing() const
{
	if (m_vSpace >= 0)
	{
		return m_vSpace;
	}
	else
	{
		return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
	}
}

int QtFlowLayout::count() const
{
	return itemList.size();
}

QLayoutItem* QtFlowLayout::itemAt(int index) const
{
	return itemList.value(index);
}

QLayoutItem* QtFlowLayout::takeAt(int index)
{
	if (index >= 0 && index < itemList.size())
	{
		return itemList.takeAt(index);
	}

	return 0;
}

Qt::Orientations QtFlowLayout::expandingDirections() const
{
	return 0;
}

bool QtFlowLayout::hasHeightForWidth() const
{
	return true;
}

int QtFlowLayout::heightForWidth(int width) const
{
	int height = doLayout(QRect(0, 0, width, 0), true);
	return height;
}

void QtFlowLayout::setGeometry(const QRect& rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect, false);
}

QSize QtFlowLayout::sizeHint() const
{
	return minimumSize();
}

QSize QtFlowLayout::minimumSize() const
{
	QSize size;
	QLayoutItem* item;
	foreach (item, itemList)
	{
		size = size.expandedTo(item->minimumSize());
	}

	size += QSize(2 * margin(), 2 * margin());
	return size;
}

int QtFlowLayout::doLayout(QRect rect, bool testOnly) const
{
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);

	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
	int x = effectiveRect.x();
	int y = effectiveRect.y();
	int lineHeight = 0;

	QLayoutItem* item;
	foreach (item, itemList)
	{
		QWidget* wid = item->widget();

		int spaceX = horizontalSpacing();
		if (spaceX == -1)
		{
			spaceX = wid->style()->layoutSpacing(
				QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
		}

		int spaceY = verticalSpacing();
		if (spaceY == -1)
		{
			spaceY = wid->style()->layoutSpacing(
				QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
		}

		int nextX = x + item->sizeHint().width() + spaceX;
		if (nextX - spaceX > effectiveRect.right() && lineHeight > 0)
		{
			x = effectiveRect.x();
			y = y + lineHeight + spaceY;
			nextX = x + item->sizeHint().width() + spaceX;
			lineHeight = 0;
		}

		if (!testOnly)
		{
			item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
		}

		x = nextX;
		lineHeight = qMax(lineHeight, item->sizeHint().height());
	}

	return y + lineHeight - rect.y() + bottom;
}

int QtFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject* parent = this->parent();
	if (!parent)
	{
		return -1;
	}
	else if (parent->isWidgetType())
	{
		QWidget* pw = static_cast<QWidget*>(parent);
		return pw->style()->pixelMetric(pm, 0, pw);
	}
	else
	{
		return static_cast<QLayout*>(parent)->spacing();
	}
}
