#ifndef QT_CONTEXT_MENU_H
#define QT_CONTEXT_MENU_H

#include <memory>
#include <QAction>
#include <QObject>

class QtContextMenu
	: public QObject
{
	Q_OBJECT

public:
	static QtContextMenu* getInstance();

	QtContextMenu();

	void showDefault(QContextMenuEvent* event, QWidget* origin);
	void showExtended(QContextMenuEvent* event, QWidget* origin, const std::vector<QAction*>& actions);

private slots:
	void undoActionTriggered();
	void redoActionTriggered();

private:
	static std::shared_ptr<QtContextMenu> s_instance;

	QAction* m_undoAction;
	QAction* m_redoAction;
};

#endif // QT_CONTEXT_MENU_H
