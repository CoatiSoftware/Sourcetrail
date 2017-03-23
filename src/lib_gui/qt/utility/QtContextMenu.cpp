#include "qt/utility/QtContextMenu.h"

#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QUrl>

#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageUndo.h"

QtContextMenu* QtContextMenu::s_instance;

QAction* QtContextMenu::s_undoAction;
QAction* QtContextMenu::s_redoAction;

QAction* QtContextMenu::s_copyFullPathAction;
QAction* QtContextMenu::s_openContainingFolderAction;

FilePath QtContextMenu::s_filePath;

QtContextMenu::QtContextMenu(QContextMenuEvent* event, QWidget* origin)
	: m_menu(origin)
	, m_point(event->globalPos())
{
	getInstance();

	addUndoActions();
}

void QtContextMenu::addAction(QAction* action)
{
	m_menu.addAction(action);
}

void QtContextMenu::enableUndo(bool enabled)
{
	s_undoAction->setEnabled(enabled);
}

void QtContextMenu::enableRedo(bool enabled)
{
	s_redoAction->setEnabled(enabled);
}

void QtContextMenu::addUndoActions()
{
	addAction(s_undoAction);
	addAction(s_redoAction);
}

void QtContextMenu::addFileActions(FilePath filePath)
{
	s_filePath = filePath;

	addAction(s_copyFullPathAction);
	addAction(s_openContainingFolderAction);
}

QtContextMenu* QtContextMenu::getInstance()
{
	if (!s_instance)
	{
		s_instance = new QtContextMenu();

		s_undoAction = new QAction(tr("Back"), s_instance);
		s_undoAction->setStatusTip(tr("Go back to last active symbol"));
		s_undoAction->setToolTip(tr("Go back to last active symbol"));
		connect(s_undoAction, SIGNAL(triggered()), s_instance, SLOT(undoActionTriggered()));

		s_redoAction = new QAction(tr("Forward"), s_instance);
		s_redoAction->setStatusTip(tr("Go forward to next active symbol"));
		s_redoAction->setToolTip(tr("Go forward to next active symbol"));
		connect(s_redoAction, SIGNAL(triggered()), s_instance, SLOT(redoActionTriggered()));

		s_copyFullPathAction = new QAction(tr("Copy Full Path"), s_instance);
		s_copyFullPathAction->setStatusTip(tr("Copies the path of this file to the clipboard"));
		s_copyFullPathAction->setToolTip(tr("Copies the path of this file to the clipboard"));
		connect(s_copyFullPathAction, SIGNAL(triggered()), s_instance, SLOT(copyFullPathActionTriggered()));

		s_openContainingFolderAction = new QAction(tr("Open Containing Folder"), s_instance);
		s_openContainingFolderAction->setStatusTip(tr("Opens the folder that contains this file"));
		s_openContainingFolderAction->setToolTip(tr("Opens the folder that contains this file"));
		connect(s_openContainingFolderAction, SIGNAL(triggered()), s_instance, SLOT(openContainingFolderActionTriggered()));
	}

	return s_instance;
}

void QtContextMenu::addSeparator()
{
	m_menu.addSeparator();
}

void QtContextMenu::show()
{
	m_menu.exec(m_point);
}

void QtContextMenu::undoActionTriggered()
{
	MessageUndo().dispatch();
}

void QtContextMenu::redoActionTriggered()
{
	MessageRedo().dispatch();
}

void QtContextMenu::copyFullPathActionTriggered()
{
	const std::string pathString = (QSysInfo::windowsVersion() != QSysInfo::WV_None) ? s_filePath.getBackslashedString() : s_filePath.str();
	QApplication::clipboard()->setText(pathString.c_str());
}

void QtContextMenu::openContainingFolderActionTriggered()
{
	FilePath dir = s_filePath.parentDirectory();
	if (dir.exists())
	{
		QDesktopServices::openUrl(QUrl(("file:///" + dir.str()).c_str(), QUrl::TolerantMode));
	}
	else
	{
		LOG_ERROR("Unable to open directory: " + dir.str());
	}
}

QtContextMenu::QtContextMenu()
{
}
