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

QTableWidget* QtKeyboardShortcuts::createGenerelShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_general");

	table->setRowCount(17);

	table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("Larger Font")));
	table->setItem(1, 0, new QTableWidgetItem(QStringLiteral("Smaller Font")));
	table->setItem(2, 0, new QTableWidgetItem(QStringLiteral("Reset Font Size")));
	table->setItem(3, 0, new QTableWidgetItem(QStringLiteral("Back")));
	table->setItem(4, 0, new QTableWidgetItem(QStringLiteral("Forward")));
	table->setItem(5, 0, new QTableWidgetItem(QStringLiteral("Refresh")));
	table->setItem(6, 0, new QTableWidgetItem(QStringLiteral("Full Refresh")));
	table->setItem(7, 0, new QTableWidgetItem(QStringLiteral("Find Symbol")));
	table->setItem(8, 0, new QTableWidgetItem(QStringLiteral("Find Text")));
	table->setItem(9, 0, new QTableWidgetItem(QStringLiteral("Find On-Screen")));
	table->setItem(10, 0, new QTableWidgetItem(QStringLiteral("New Project")));
	table->setItem(11, 0, new QTableWidgetItem(QStringLiteral("Open Project")));
	table->setItem(12, 0, new QTableWidgetItem(QStringLiteral("Close Window")));
	table->setItem(13, 0, new QTableWidgetItem(QStringLiteral("Hide Window")));
	table->setItem(14, 0, new QTableWidgetItem(QStringLiteral("To Overview")));
	table->setItem(15, 0, new QTableWidgetItem(QStringLiteral("Preferences")));
	table->setItem(16, 0, new QTableWidgetItem(QStringLiteral("Bookmark Active Symbol")));
	table->setItem(17, 0, new QTableWidgetItem(QStringLiteral("Bookmark Manager")));

#if defined(Q_OS_MAC)
	table->setItem(0, 1, new QTableWidgetItem("Cmd + +"));
	table->setItem(1, 1, new QTableWidgetItem("Cmd + -"));
	table->setItem(2, 1, new QTableWidgetItem("Cmd + 0"));
	table->setItem(3, 1, new QTableWidgetItem("Cmd + Z | Backspace"));
	table->setItem(4, 1, new QTableWidgetItem("Cmd + Shift + Z"));
	table->setItem(5, 1, new QTableWidgetItem("Cmd + R"));
	table->setItem(6, 1, new QTableWidgetItem("Cmd + Shift + R"));
	table->setItem(7, 1, new QTableWidgetItem("Cmd + F"));
	table->setItem(8, 1, new QTableWidgetItem("Cmd + Shift + F"));
	table->setItem(9, 1, new QTableWidgetItem("Cmd + D | /"));
	table->setItem(10, 1, new QTableWidgetItem("Cmd + N"));
	table->setItem(11, 1, new QTableWidgetItem("Cmd + O"));
	table->setItem(12, 1, new QTableWidgetItem("Cmd + W"));
	table->setItem(13, 1, new QTableWidgetItem("Cmd + H"));
	table->setItem(14, 1, new QTableWidgetItem("Cmd + Home | Cmd + Up"));
	table->setItem(15, 1, new QTableWidgetItem("Cmd + ,"));
	table->setItem(16, 1, new QTableWidgetItem("Cmd + S"));
	table->setItem(17, 1, new QTableWidgetItem("Cmd + B"));
#else
	table->setItem(0, 1, new QTableWidgetItem(QStringLiteral("Ctrl + +")));
	table->setItem(1, 1, new QTableWidgetItem(QStringLiteral("Ctrl + -")));
	table->setItem(2, 1, new QTableWidgetItem(QStringLiteral("Ctrl + 0")));
	table->setItem(3, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Z | Backspace")));
	table->setItem(4, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Shift + Z")));
	table->setItem(5, 1, new QTableWidgetItem(QStringLiteral("F5")));
	table->setItem(6, 1, new QTableWidgetItem(QStringLiteral("Shift + F5")));
	table->setItem(7, 1, new QTableWidgetItem(QStringLiteral("Ctrl + F")));
	table->setItem(8, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Shift + F")));
	table->setItem(9, 1, new QTableWidgetItem(QStringLiteral("Ctrl + D | /")));
	table->setItem(10, 1, new QTableWidgetItem(QStringLiteral("Ctrl + N")));
	table->setItem(11, 1, new QTableWidgetItem(QStringLiteral("Ctrl + O")));
#	if defined(Q_OS_WIN32)
	table->setItem(12, 1, new QTableWidgetItem("Alt + F4"));
#	else
	table->setItem(12, 1, new QTableWidgetItem(QStringLiteral("Ctrl + W")));
#	endif
	table->setItem(13, 1, new QTableWidgetItem(QLatin1String("")));
	table->setItem(14, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Home")));
	table->setItem(15, 1, new QTableWidgetItem(QStringLiteral("Ctrl + ,")));
	table->setItem(16, 1, new QTableWidgetItem(QStringLiteral("Ctrl + S")));
	table->setItem(17, 1, new QTableWidgetItem(QStringLiteral("Ctrl + B")));
#endif

	table->updateSize();

	return table;
}

QTableWidget* QtKeyboardShortcuts::createCodeViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_code");

	table->setRowCount(4);
	table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("Next Reference")));
	table->setItem(1, 0, new QTableWidgetItem(QStringLiteral("Previous Reference")));
	table->setItem(2, 0, new QTableWidgetItem(QStringLiteral("Next Local Reference")));
	table->setItem(3, 0, new QTableWidgetItem(QStringLiteral("Previous Local Reference")));

#if defined(Q_OS_MAC)
	table->setItem(0, 1, new QTableWidgetItem("Cmd + G"));
	table->setItem(1, 1, new QTableWidgetItem("Cmd + Shift + G"));
	table->setItem(2, 1, new QTableWidgetItem("Cmd + T"));
	table->setItem(3, 1, new QTableWidgetItem("Cmd + Shift + T"));
#else
	table->setItem(0, 1, new QTableWidgetItem(QStringLiteral("Ctrl + G")));
	table->setItem(1, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Shift + G")));
	table->setItem(2, 1, new QTableWidgetItem(QStringLiteral("Ctrl + T")));
	table->setItem(3, 1, new QTableWidgetItem(QStringLiteral("Ctrl + Shift + T")));
#endif

	table->updateSize();

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGraphViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_graph");

	table->setRowCount(7);
	table->setItem(0, 0, new QTableWidgetItem(QStringLiteral("Pan left")));
	table->setItem(1, 0, new QTableWidgetItem(QStringLiteral("Pan right")));
	table->setItem(2, 0, new QTableWidgetItem(QStringLiteral("Pan up")));
	table->setItem(3, 0, new QTableWidgetItem(QStringLiteral("Pan down")));
	table->setItem(4, 0, new QTableWidgetItem(QStringLiteral("Zoom in")));
	table->setItem(5, 0, new QTableWidgetItem(QStringLiteral("Zoom out")));
	table->setItem(6, 0, new QTableWidgetItem(QStringLiteral("Reset Zoom")));

	table->setItem(0, 1, new QTableWidgetItem(QStringLiteral("A")));
	table->setItem(1, 1, new QTableWidgetItem(QStringLiteral("D")));
	table->setItem(2, 1, new QTableWidgetItem(QStringLiteral("W")));
	table->setItem(3, 1, new QTableWidgetItem(QStringLiteral("S")));
#if defined(Q_OS_MAC)
	table->setItem(4, 1, new QTableWidgetItem("Shift + W | Cmd + Mousewheel up"));
	table->setItem(5, 1, new QTableWidgetItem("Shift + S | Cmd + Mousewheel down"));
#else
	table->setItem(4, 1, new QTableWidgetItem(QStringLiteral("Shift + W | Ctrl + Mousewheel up")));
	table->setItem(5, 1, new QTableWidgetItem(QStringLiteral("Shift + S | Ctrl + Mousewheel down")));
#endif
	table->setItem(6, 1, new QTableWidgetItem(QStringLiteral("0")));

	table->updateSize();

	return table;
}
