#include "QtTabBar.h"
#include <QtContextMenu.h>
#include <QtGraphicsView.h>

QtTabBar::QtTabBar(QWidget* parent): QTabBar(parent)
{
	setFocusPolicy(Qt::NoFocus);
    m_closeTabsToRight = new QAction("Close tabs to the right", this);
}

QSize QtTabBar::minimumSizeHint() const
{
	return QSize(0, QTabBar::minimumSizeHint().height());
}

QSize QtTabBar::tabSizeHint(int index) const
{
	return QSize(300, QTabBar::tabSizeHint(index).height());
}

QSize QtTabBar::minimumTabSizeHint(int index) const
{
	return QSize(45, QTabBar::minimumTabSizeHint(index).height());
}

void QtTabBar::closeTabsToRight(int tabNum)
{
    tabNum = 42;
    emit signalCloseTabsToRight(tabNum);
}

void QtTabBar::contextMenuEvent(QContextMenuEvent* event)
{
    QtContextMenu menu(event, this);
	menu.addAction(m_closeTabsToRight);
    connect(m_closeTabsToRight, &QtTabBar::closeTabsToRight, this, &QtGraphicsView::closeTabsToRight);
    menu.show();
    return;
}
