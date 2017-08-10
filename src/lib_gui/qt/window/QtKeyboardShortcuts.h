#ifndef QT_KEYBOARD_SHORTCUTS_H
#define QT_KEYBOARD_SHORTCUTS_H

#include <QScrollArea>
#include <QTableWidget>

#include "qt/window/QtWindow.h"

class QtShortcutTable
	: public QTableWidget
{
	Q_OBJECT
public:
	QtShortcutTable(QWidget* parent = nullptr);
	void updateSize();

protected:
	virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
};

class QtKeyboardShortcuts
	: public QtWindow
{
	Q_OBJECT
public:
	QtKeyboardShortcuts(QWidget* parent = 0);
	virtual ~QtKeyboardShortcuts();

	QSize sizeHint() const override;

	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

private:
	QtShortcutTable* createTableWidget(const std::string& objectName);

	QTableWidget* createGenerelShortcutsTable();
	QTableWidget* createCodeViewShortcutsTable();
	QTableWidget* createGraphViewShortcutsTable();
};

#endif // QT_KEYBOARD_SHORTCUTS_H
