#ifndef QT_CONTEXT_MENU_H
#define QT_CONTEXT_MENU_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <memory>

#include "FilePath.h"

class QtContextMenu: public QObject
{
	Q_OBJECT

public:
	QtContextMenu(QContextMenuEvent* event, QWidget* origin);

	void addAction(QAction* action);
	void addUndoActions();
	void addFileActions(const FilePath& filePath);

	static QtContextMenu* getInstance();

	void addSeparator();

	void show();

	void enableUndo(bool enabled);
	void enableRedo(bool enabled);

private slots:
	void undoActionTriggered();
	void redoActionTriggered();

	void copyFullPathActionTriggered();
	void openContainingFolderActionTriggered();

private:
	QtContextMenu();

	static QtContextMenu* s_instance;

	static QAction* s_undoAction;
	static QAction* s_redoAction;

	static QAction* s_copyFullPathAction;
	static QAction* s_openContainingFolderAction;

	static FilePath s_filePath;

	QMenu m_menu;
	QPoint m_point;
};

#endif	  // QT_CONTEXT_MENU_H
