#ifndef QT_KEYBOARD_SHORTCUTS_H
#define QT_KEYBOARD_SHORTCUTS_H

#include <QScrollArea>
#include <QTableWidget>

#include "qt/window/QtWindow.h"

class QtKeyboardShortcuts
	: public QtWindow
{
	Q_OBJECT
public:
	QtKeyboardShortcuts(QWidget* parent = 0);
	virtual ~QtKeyboardShortcuts();

	QSize sizeHint() const override;

	void setupKeyboardShortcuts();

private slots:
	void handleCloseButtonClicked();

private:
	QTableWidget* createTableWidget(const std::string& objectName);

	QTableWidget* createGenerelShortcutsTable();
	QTableWidget* createCodeViewShortcutsTable();
	QTableWidget* createGraphViewShortcutsTable();
};

#endif // QT_KEYBOARD_SHORTCUTS_H