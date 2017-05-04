#include "QtKeyboardShortcuts.h"

#include <QHeaderView>
#include <QLabel>

#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtShortcutTable::QtShortcutTable(QWidget* parent)
	: QTableWidget(parent)
{
}

void QtShortcutTable::updateSize()
{
	int height = rowCount() * rowHeight(0) + horizontalHeader()->height() + 2 * frameWidth() + 8;
	setMinimumHeight(height);
	setMaximumHeight(height);
}

void QtShortcutTable::wheelEvent(QWheelEvent *event)
{
	event->ignore();
}


QtKeyboardShortcuts::QtKeyboardShortcuts(QWidget* parent)
	: QtWindow(parent)
{
	setScrollAble(true);

	raise();
}

QtKeyboardShortcuts::~QtKeyboardShortcuts()
{
}

QSize QtKeyboardShortcuts::sizeHint() const
{
	return QSize(666, 666);
}

void QtKeyboardShortcuts::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	QLabel* generelLabel = new QLabel(this);
	generelLabel->setObjectName("general_label");
	generelLabel->setText("General Shortcuts");
	layout->addWidget(generelLabel);

	layout->addWidget(createGenerelShortcutsTable());

	layout->addSpacing(20);

	QLabel* codeLabel = new QLabel(this);
	codeLabel->setObjectName("code_label");
	codeLabel->setText("Code View Shortcuts");
	layout->addWidget(codeLabel);

	layout->addWidget(createCodeViewShortcutsTable());

	layout->addSpacing(20);

	QLabel* graphLabel = new QLabel(this);
	graphLabel->setObjectName("graph_label");
	graphLabel->setText("Graph View Shortcuts");
	layout->addWidget(graphLabel);

	layout->addWidget(createGraphViewShortcutsTable());

	widget->setLayout(layout);

	widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("keyboard_shortcuts/keyboard_shortcuts.css"))).c_str());
}

void QtKeyboardShortcuts::windowReady()
{
	updateTitle("Keyboard Shortcuts");
	updateCloseButton("Close");

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
	table->setHorizontalHeaderItem(0, new QTableWidgetItem("Command"));
	table->setHorizontalHeaderItem(1, new QTableWidgetItem("Shortcut"));

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGenerelShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_general");

	table->setRowCount(15);

	table->setItem(0, 0, new QTableWidgetItem("Larger Font"));
	table->setItem(1, 0, new QTableWidgetItem("Smaller Font"));
	table->setItem(2, 0, new QTableWidgetItem("Reset Font Size"));
	table->setItem(3, 0, new QTableWidgetItem("Back"));
	table->setItem(4, 0, new QTableWidgetItem("Forward"));
	table->setItem(5, 0, new QTableWidgetItem("Refresh"));
	table->setItem(6, 0, new QTableWidgetItem("Force Refresh"));
	table->setItem(7, 0, new QTableWidgetItem("Find Symbol"));
	table->setItem(8, 0, new QTableWidgetItem("Find Text"));
	table->setItem(9, 0, new QTableWidgetItem("New Project"));
	table->setItem(10, 0, new QTableWidgetItem("Open Project"));
	table->setItem(11, 0, new QTableWidgetItem("Close Window"));
	table->setItem(12, 0, new QTableWidgetItem("Hide Window"));
	table->setItem(13, 0, new QTableWidgetItem("To Overview"));
	table->setItem(14, 0, new QTableWidgetItem("Preferences"));

#if defined(Q_OS_WIN32)
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + +"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + -"));
	table->setItem(2, 1, new QTableWidgetItem("Ctrl + 0"));
	table->setItem(3, 1, new QTableWidgetItem("Ctrl + Z | Backspace"));
	table->setItem(4, 1, new QTableWidgetItem("Ctrl + Shift + Z"));
	table->setItem(5, 1, new QTableWidgetItem("F5"));
	table->setItem(6, 1, new QTableWidgetItem("Shift + F5"));
	table->setItem(7, 1, new QTableWidgetItem("Ctrl + F"));
	table->setItem(8, 1, new QTableWidgetItem("Ctrl + Shift + F"));
	table->setItem(9, 1, new QTableWidgetItem("Ctrl + N"));
	table->setItem(10, 1, new QTableWidgetItem("Ctrl + O"));
	table->setItem(11, 1, new QTableWidgetItem("Alt + F4"));
	table->setItem(12, 1, new QTableWidgetItem(""));
	table->setItem(13, 1, new QTableWidgetItem("Ctrl + Home"));
	table->setItem(14, 1, new QTableWidgetItem("Ctrl + ,"));
#elif defined(Q_OS_LINUX)
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + +"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + -"));
	table->setItem(2, 1, new QTableWidgetItem("Ctrl + 0"));
	table->setItem(3, 1, new QTableWidgetItem("Ctrl + Z | Backspace"));
	table->setItem(4, 1, new QTableWidgetItem("Ctrl + Shift + Z"));
	table->setItem(5, 1, new QTableWidgetItem("F5"));
	table->setItem(6, 1, new QTableWidgetItem("Shift + F5"));
	table->setItem(7, 1, new QTableWidgetItem("Ctrl + F"));
	table->setItem(8, 1, new QTableWidgetItem("Ctrl + Shift + F"));
	table->setItem(9, 1, new QTableWidgetItem("Ctrl + N"));
	table->setItem(10, 1, new QTableWidgetItem("Ctrl + O"));
	table->setItem(11, 1, new QTableWidgetItem("Ctrl + W"));
	table->setItem(12, 1, new QTableWidgetItem(""));
	table->setItem(13, 1, new QTableWidgetItem("Ctrl + Home"));
	table->setItem(14, 1, new QTableWidgetItem("Ctrl + ,"));
#elif defined(Q_OS_MAC)
	table->setItem(0, 1, new QTableWidgetItem("Cmd + +"));
	table->setItem(1, 1, new QTableWidgetItem("Cmd + -"));
	table->setItem(2, 1, new QTableWidgetItem("Cmd + 0"));
	table->setItem(3, 1, new QTableWidgetItem("Cmd + Z | Backspace"));
	table->setItem(4, 1, new QTableWidgetItem("Cmd + Shift + Z"));
	table->setItem(5, 1, new QTableWidgetItem("Cmd + R"));
	table->setItem(6, 1, new QTableWidgetItem("Cmd + Shift + R"));
	table->setItem(7, 1, new QTableWidgetItem("Cmd + F"));
	table->setItem(8, 1, new QTableWidgetItem("Cmd + Shift + F"));
	table->setItem(9, 1, new QTableWidgetItem("Cmd + N"));
	table->setItem(10, 1, new QTableWidgetItem("Cmd + O"));
	table->setItem(11, 1, new QTableWidgetItem("Cmd + W"));
	table->setItem(12, 1, new QTableWidgetItem("Cmd + H"));
	table->setItem(13, 1, new QTableWidgetItem("Cmd + Home | Cmd + Up"));
	table->setItem(14, 1, new QTableWidgetItem("Cmd + ,"));
#else
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + +"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + -"));
	table->setItem(2, 1, new QTableWidgetItem("Ctrl + 0"));
	table->setItem(3, 1, new QTableWidgetItem("Ctrl + Z | Backspace"));
	table->setItem(4, 1, new QTableWidgetItem("Ctrl + Shift + Z"));
	table->setItem(5, 1, new QTableWidgetItem("F5"));
	table->setItem(6, 1, new QTableWidgetItem("Shift + F5"));
	table->setItem(7, 1, new QTableWidgetItem("Ctrl + F"));
	table->setItem(8, 1, new QTableWidgetItem("Ctrl + Shift + F"));
	table->setItem(9, 1, new QTableWidgetItem("Ctrl + N"));
	table->setItem(10, 1, new QTableWidgetItem("Ctrl + O"));
	table->setItem(11, 1, new QTableWidgetItem("Alt + F4"));
	table->setItem(12, 1, new QTableWidgetItem(""));
	table->setItem(13, 1, new QTableWidgetItem("Ctrl + Home"));
	table->setItem(14, 1, new QTableWidgetItem("Ctrl + ,"));
#endif

	table->updateSize();

	return table;
}

QTableWidget* QtKeyboardShortcuts::createCodeViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_code");

	table->setRowCount(2);
	table->setItem(0, 0, new QTableWidgetItem("Code Reference Next"));
	table->setItem(1, 0, new QTableWidgetItem("Code Reference Previous"));

#if defined(Q_OS_WIN32)
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + G"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + Shift + G"));
#elif defined(Q_OS_LINUX)
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + G"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + Shift + G"));
#elif defined(Q_OS_MAC)
	table->setItem(0, 1, new QTableWidgetItem("Cmd + G"));
	table->setItem(1, 1, new QTableWidgetItem("Cmd + Shift + G"));
#else
	table->setItem(0, 1, new QTableWidgetItem("Ctrl + G"));
	table->setItem(1, 1, new QTableWidgetItem("Ctrl + Shift + G"));
#endif

	table->updateSize();

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGraphViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_graph");

	table->setRowCount(7);
	table->setItem(0, 0, new QTableWidgetItem("Pan left"));
	table->setItem(1, 0, new QTableWidgetItem("Pan right"));
	table->setItem(2, 0, new QTableWidgetItem("Pan up"));
	table->setItem(3, 0, new QTableWidgetItem("Pan down"));
	table->setItem(4, 0, new QTableWidgetItem("Zoom in"));
	table->setItem(5, 0, new QTableWidgetItem("Zoom out"));
	table->setItem(6, 0, new QTableWidgetItem("Reset Zoom"));

#if defined(Q_OS_WIN32)
	table->setItem(0, 1, new QTableWidgetItem("A"));
	table->setItem(1, 1, new QTableWidgetItem("D"));
	table->setItem(2, 1, new QTableWidgetItem("W"));
	table->setItem(3, 1, new QTableWidgetItem("S"));
	table->setItem(4, 1, new QTableWidgetItem("Shift + W | Shift + Mousewheel up"));
	table->setItem(5, 1, new QTableWidgetItem("Shift + S | Shift + Mousewheel down"));
	table->setItem(6, 1, new QTableWidgetItem("0"));
#elif defined(Q_OS_LINUX)
	table->setItem(0, 1, new QTableWidgetItem("A"));
	table->setItem(1, 1, new QTableWidgetItem("D"));
	table->setItem(2, 1, new QTableWidgetItem("W"));
	table->setItem(3, 1, new QTableWidgetItem("S"));
	table->setItem(4, 1, new QTableWidgetItem("Shift + W | Shift + Mousewheel up"));
	table->setItem(5, 1, new QTableWidgetItem("Shift + S | Shift + Mousewheel down"));
	table->setItem(6, 1, new QTableWidgetItem("0"));
#elif defined(Q_OS_MAC)
	table->setItem(0, 1, new QTableWidgetItem("A"));
	table->setItem(1, 1, new QTableWidgetItem("D"));
	table->setItem(2, 1, new QTableWidgetItem("W"));
	table->setItem(3, 1, new QTableWidgetItem("S"));
	table->setItem(4, 1, new QTableWidgetItem("Shift + W | Shift + Mousewheel up"));
	table->setItem(5, 1, new QTableWidgetItem("Shift + S | Shift + Mousewheel down"));
	table->setItem(6, 1, new QTableWidgetItem("0"));
#else
	table->setItem(0, 1, new QTableWidgetItem("A"));
	table->setItem(1, 1, new QTableWidgetItem("D"));
	table->setItem(2, 1, new QTableWidgetItem("W"));
	table->setItem(3, 1, new QTableWidgetItem("S"));
	table->setItem(4, 1, new QTableWidgetItem("Shift + W | Shift + Mousewheel up"));
	table->setItem(5, 1, new QTableWidgetItem("Shift + S | Shift + Mousewheel down"));
	table->setItem(6, 1, new QTableWidgetItem("0"));
#endif

	table->updateSize();

	return table;
}
