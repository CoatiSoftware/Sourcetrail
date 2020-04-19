#ifndef QT_KEYBOARD_SHORTCUTS_H
#define QT_KEYBOARD_SHORTCUTS_H

#include <QScrollArea>
#include <QTableWidget>

#include "QtWindow.h"

class QtShortcutTable: public QTableWidget
{
	Q_OBJECT
public:
	QtShortcutTable(QWidget* parent = nullptr);
	void updateSize();

protected:
	virtual void wheelEvent(QWheelEvent* event) override;
};

class QtKeyboardShortcuts: public QtWindow
{
	Q_OBJECT
public:
	QtKeyboardShortcuts(QWidget* parent = 0);
	virtual ~QtKeyboardShortcuts();

	QSize sizeHint() const override;

	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

private:
	struct Shortcut
	{
		const QString name;
		const QString shortcut;

		Shortcut(const QString& name, const QString& shortcut);
		static Shortcut defaultOrMac(
			const QString& name, const QString& defaultShortcut, const QString& macShortcut);
		static Shortcut winMacOrLinux(
			const QString& name,
			const QString& winShortcut,
			const QString& macShortcut,
			const QString& linuxShortcut);
	};

	QtShortcutTable* createTableWidget(const std::string& objectName);
	void addShortcuts(QtShortcutTable* table, const std::vector<Shortcut>& shortcuts) const;

	QTableWidget* createGenerelShortcutsTable();
	QTableWidget* createCodeViewShortcutsTable();
	QTableWidget* createGraphViewShortcutsTable();
};

#endif	  // QT_KEYBOARD_SHORTCUTS_H
