#include "QtKeyboardShortcuts.h"

#include <QHeaderView>
#include <QLabel>

#include "ResourcePaths.h"
#include "utilityQt.h"

QtShortcutTable::QtShortcutTable(QWidget* parent): QTableWidget(parent) {}

void QtShortcutTable::updateSize()
{
	int height = rowCount() * rowHeight(0) + horizontalHeader()->height() + 2 * frameWidth() + 8;
	setMinimumHeight(height);
	setMaximumHeight(height);
}

void QtShortcutTable::wheelEvent(QWheelEvent* event)
{
	event->ignore();
}


QtKeyboardShortcuts::QtKeyboardShortcuts(QWidget* parent): QtWindow(false, parent)
{
	setScrollAble(true);
}

QtKeyboardShortcuts::~QtKeyboardShortcuts() {}

QSize QtKeyboardShortcuts::sizeHint() const
{
	return QSize(666, 666);
}

void QtKeyboardShortcuts::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	QLabel* generelLabel = new QLabel(this);
	generelLabel->setObjectName(QStringLiteral("general_label"));
	generelLabel->setText(QStringLiteral("General Shortcuts"));
	layout->addWidget(generelLabel);

	layout->addWidget(createGenerelShortcutsTable());

	layout->addSpacing(20);

	QLabel* codeLabel = new QLabel(this);
	codeLabel->setObjectName(QStringLiteral("code_label"));
	codeLabel->setText(QStringLiteral("Code View Shortcuts"));
	layout->addWidget(codeLabel);

	layout->addWidget(createCodeViewShortcutsTable());

	layout->addSpacing(20);

	QLabel* graphLabel = new QLabel(this);
	graphLabel->setObjectName(QStringLiteral("graph_label"));
	graphLabel->setText(QStringLiteral("Graph View Shortcuts"));
	layout->addWidget(graphLabel);

	layout->addWidget(createGraphViewShortcutsTable());

	widget->setLayout(layout);

	widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(
													 L"keyboard_shortcuts/keyboard_shortcuts.css"))
							  .c_str());
}

void QtKeyboardShortcuts::windowReady()
{
	updateTitle(QStringLiteral("Keyboard Shortcuts"));
	updateCloseButton(QStringLiteral("Close"));

	setNextVisible(false);
	setPreviousVisible(false);
}

QtKeyboardShortcuts::Shortcut::Shortcut(const QString& name, const QString& shortcut)
	: name(name), shortcut(shortcut)
{
}

QtKeyboardShortcuts::Shortcut QtKeyboardShortcuts::Shortcut::defaultOrMac(
	const QString& name, const QString& defaultShortcut, const QString& macShortcut)
{
#if defined(Q_OS_MAC)
	return {name, macShortcut};
#else
	return {name, defaultShortcut};
#endif
}

QtKeyboardShortcuts::Shortcut QtKeyboardShortcuts::Shortcut::winMacOrLinux(
	const QString& name,
	const QString& winShortcut,
	const QString& macShortcut,
	const QString& linuxShortcut)
{
#if defined(Q_OS_WIN32)
	return {name, winShortcut};
#elif defined(Q_OS_MAC)
	return {name, macShortcut};
#else
	return {name, linuxShortcut};
#endif
}

QtShortcutTable* QtKeyboardShortcuts::createTableWidget(const std::string& objectName)
{
	QtShortcutTable* table = new QtShortcutTable(this);
	table->setObjectName(objectName.c_str());

	table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);

	table->setShowGrid(true);
	table->setAlternatingRowColors(true);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	table->verticalHeader()->hide();

	table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	table->setColumnCount(2);
	table->setHorizontalHeaderItem(0, new QTableWidgetItem(QStringLiteral("Command")));
	table->setHorizontalHeaderItem(1, new QTableWidgetItem(QStringLiteral("Shortcut")));

	return table;
}

void QtKeyboardShortcuts::addShortcuts(QtShortcutTable* table, const std::vector<Shortcut>& shortcuts) const
{
	table->setRowCount(static_cast<int>(shortcuts.size()));

	for (size_t i = 0; i < shortcuts.size(); ++i)
	{
		table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(shortcuts[i].name));
		table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(shortcuts[i].shortcut));
	}

	table->updateSize();
}

QTableWidget* QtKeyboardShortcuts::createGenerelShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_general");

	addShortcuts(
		table,
		{Shortcut(QStringLiteral("Switch Focus between Graph and Code"), QStringLiteral("Tab")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Larger Font"), QStringLiteral("Ctrl + +"), QStringLiteral("Cmd + +")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Smaller Font"), QStringLiteral("Ctrl + -"), QStringLiteral("Cmd + -")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Reset Font Size"), QStringLiteral("Ctrl + 0"), QStringLiteral("Cmd + 0")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Back"),
			 QStringLiteral("Alt + Left | Z | Y | Backspace"),
			 QStringLiteral("Cmd + [ | Z | Y | Backspace")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Forward"),
			 QStringLiteral("Alt + Right | Shift + Z | Shift + Y"),
			 QStringLiteral("Cmd + ] | Shift + Z | Shift + Y")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Refresh"), QStringLiteral("F5"), QStringLiteral("Cmd + R")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Full Refresh"),
			 QStringLiteral("Shift + F5"),
			 QStringLiteral("Cmd + Shift + R")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Find Symbol"), QStringLiteral("Ctrl + F"), QStringLiteral("Cmd + F")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Find Text"),
			 QStringLiteral("Ctrl + Shift + F"),
			 QStringLiteral("Cmd + Shift + F")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Find On-Screen"),
			 QStringLiteral("Ctrl + D | /"),
			 QStringLiteral("Cmd + D | /")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("New Project"), QStringLiteral("Ctrl + N"), QStringLiteral("Cmd + N")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Open Project"), QStringLiteral("Ctrl + O"), QStringLiteral("Cmd + O")),
		 Shortcut::winMacOrLinux(
			 QStringLiteral("Close Window"),
			 QStringLiteral("Alt + F4"),
			 QStringLiteral("Cmd + W"),
			 QStringLiteral("Ctrl + W")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Hide Window"), QStringLiteral(""), QStringLiteral("Cmd + H")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("To Overview"),
			 QStringLiteral("Ctrl + Home"),
			 QStringLiteral("Cmd + Home | Cmd + Up")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Preferences"), QStringLiteral("Ctrl + ,"), QStringLiteral("Cmd + ,")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Bookmark Active Symbol"),
			 QStringLiteral("Ctrl + S"),
			 QStringLiteral("Cmd + S")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Bookmark Manager"),
			 QStringLiteral("Ctrl + B"),
			 QStringLiteral("Cmd + B"))});

	return table;
}

QTableWidget* QtKeyboardShortcuts::createCodeViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_code");

	addShortcuts(
		table,
		{Shortcut(QStringLiteral("Move Focus Within Code"), QStringLiteral("WASD | HJKL | Arrows")),
		 Shortcut(
			 QStringLiteral("Move Focus to Closest Reference"),
			 QStringLiteral("Shift + WASD | Shift + HJKL | Shift + Arrows")),
		 Shortcut(QStringLiteral("Activate Location"), QStringLiteral("Enter | E")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Activate Location in New Tab"),
			 QStringLiteral("Ctrl + Shift + Enter | Ctrl + Shift + E"),
			 QStringLiteral("Cmd + Shift + Enter | Cmd + Shift + E")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Next Reference"), QStringLiteral("Ctrl + G"), QStringLiteral("Cmd + G")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Previous Reference"),
			 QStringLiteral("Ctrl + Shift + G"),
			 QStringLiteral("Cmd + Shift + G")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Next Local Reference"),
			 QStringLiteral("Ctrl + L"),
			 QStringLiteral("Cmd + L")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Previous Local Reference"),
			 QStringLiteral("Ctrl + Shift + L"),
			 QStringLiteral("Cmd + Shift + L")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Scroll Code Area"),
			 QStringLiteral("Ctrl + Arrows"),
			 QStringLiteral("Cmd + Arrows"))});

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGraphViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_graph");

	addShortcuts(
		table,
		{Shortcut(QStringLiteral("Move Focus Within Nodes"), QStringLiteral("WASD | HJKL | Arrows")),
		 Shortcut(
			 QStringLiteral("Move Focus Within Edges"),
			 QStringLiteral("Shift + WASD | Shift + HJKL | Shift + Arrows")),
		 Shortcut(QStringLiteral("Activate Node/Edge"), QStringLiteral("Enter | E")),
		 Shortcut(QStringLiteral("Expand/Collapse Node"), QStringLiteral("Shift + Enter | Shift + E")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Activate Node in New Tab"),
			 QStringLiteral("Ctrl + Shift + Enter | Ctrl + Shift + E"),
			 QStringLiteral("Cmd + Shift + Enter | Cmd + Shift + E")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Scroll Graph Area"),
			 QStringLiteral("Ctrl + Arrows"),
			 QStringLiteral("Cmd + Arrows")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Zoom in"),
			 QStringLiteral("Ctrl + Shift + Up | Ctrl + Mouse Wheel Up"),
			 QStringLiteral("Cmd + Shift + Up | Cmd + Mouse Wheel Up")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Zoom out"),
			 QStringLiteral("Ctrl + Shift + Down | Ctrl + Mouse Wheel Down"),
			 QStringLiteral("Cmd + Shift + Down | Cmd + Mouse Wheel Down")),
		 Shortcut(QStringLiteral("Reset Zoom"), QStringLiteral("0")),
		 Shortcut::defaultOrMac(
			 QStringLiteral("Open Custom Trail Dialog"),
			 QStringLiteral("Ctrl + U"),
			 QStringLiteral("Cmd + U"))});

	return table;
}
