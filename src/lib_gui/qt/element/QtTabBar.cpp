#include "QtTabBar.h"

#include <QContextMenuEvent>

#include "QtContextMenu.h"
#include "QtGraphicsView.h"
#include "logging.h"


QtTabBar::QtTabBar(QWidget* parent): QTabBar(parent)
{
	setFocusPolicy(Qt::NoFocus);
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

void QtTabBar::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu menu(event, this);
	QAction* m_closeTabsToRight = new QAction(QStringLiteral("Close tabs to the right"), this);
	menu.addAction(m_closeTabsToRight);

	connect(m_closeTabsToRight, &QAction::triggered, this, [&]() {
		// We dont want to close tabs right of the current active tab.
		// No, our intend is to close tabs right of the currently hovered tab.
		auto tabNum = tabAt(event->pos());
		LOG_INFO(
			"Handling closeTabs... emitting signal to close tabs right of tab nr. " +
			std::to_string(tabNum));
		emit signalCloseTabsToRight(tabNum);
	});

	menu.show();
	return;
}
