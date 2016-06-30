#include "qt/utility/QtContextMenu.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageUndo.h"

std::shared_ptr<QtContextMenu> QtContextMenu::s_instance;

QtContextMenu* QtContextMenu::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::make_shared<QtContextMenu>();
	}

	return s_instance.get();
}

QtContextMenu::QtContextMenu()
{
	m_undoAction = new QAction(tr("Back"), this);
	m_undoAction->setStatusTip(tr("Go back to last active symbol"));
	m_undoAction->setToolTip(tr("Go back to last active symbol"));
	connect(m_undoAction, SIGNAL(triggered()), this, SLOT(undoActionTriggered()));

	m_redoAction = new QAction(tr("Forward"), this);
	m_redoAction->setStatusTip(tr("Go forward to next active symbol"));
	m_redoAction->setToolTip(tr("Go forward to next active symbol"));
	connect(m_redoAction, SIGNAL(triggered()), this, SLOT(redoActionTriggered()));
}

void QtContextMenu::showDefault(QContextMenuEvent* event, QWidget* origin)
{
	showExtended(event, origin, std::vector<QAction*>());
}

void QtContextMenu::showExtended(QContextMenuEvent* event, QWidget* origin, const std::vector<QAction*>& actions)
{
	QMenu menu(origin);

	menu.addAction(m_undoAction);
	menu.addAction(m_redoAction);

	if (actions.size())
	{
		menu.addSeparator();

		for (QAction* action : actions)
		{
			menu.addAction(action);
		}
	}

	menu.exec(event->globalPos());
}

void QtContextMenu::undoActionTriggered()
{
	MessageUndo().dispatch();
}

void QtContextMenu::redoActionTriggered()
{
	MessageRedo().dispatch();
}
