#include "QtKeyboardShortcuts.h"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QTableView>
#include <QtGlobal>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtKeyboardShortcuts::QtKeyboardShortcuts(QWidget* parent)
{
}

QtKeyboardShortcuts::~QtKeyboardShortcuts()
{
}

QSize QtKeyboardShortcuts::sizeHint() const
{
	return QSize(666, 666);
}

void QtKeyboardShortcuts::setupKeyboardShortcuts()
{
	QVBoxLayout* layout = new QVBoxLayout();

	QLabel* title = new QLabel(this);
	title->setObjectName("title");
	title->setText("Keyboard Shortcuts");
	layout->addWidget(title);

	QWidget* container = new QWidget();
	container->setObjectName("container");
	QVBoxLayout* scrollLayout = new QVBoxLayout(container);

	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setObjectName("scroll_area");
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(container);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	layout->addWidget(scrollArea);

	QLabel* generelLabel = new QLabel(this);
	generelLabel->setObjectName("general_label");
	generelLabel->setText("General Shortcuts");
	scrollLayout->addWidget(generelLabel);

	scrollLayout->addWidget(createGenerelShortcutsTable());

	QLabel* codeLabel = new QLabel(this);
	codeLabel->setObjectName("code_label");
	codeLabel->setText("Code View Shortcuts");
	scrollLayout->addWidget(codeLabel);

	scrollLayout->addWidget(createCodeViewShortcutsTable());

	QLabel* graphLabel = new QLabel(this);
	graphLabel->setObjectName("graph_label");
	graphLabel->setText("Graph View Shortcuts");
	scrollLayout->addWidget(graphLabel);

	scrollLayout->addWidget(createGraphViewShortcutsTable());

	QPushButton* closeButton = new QPushButton(this);
	closeButton->setObjectName("close_button");
	closeButton->setText("Close");
	closeButton->show();
	layout->addWidget(closeButton);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(handleCloseButtonClicked()));

	setLayout(layout);

	setFixedSize(QSize(682, 700));

	// setFixedSize(table->horizontalHeader()->length(), table->verticalHeader()->length());

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "keyboard_shortcuts/keyboard_shortcuts.css").c_str());
}

void QtKeyboardShortcuts::handleCloseButtonClicked()
{
	emit canceled();
}

QTableWidget* QtKeyboardShortcuts::createTableWidget(const std::string& objectName)
{
	QTableWidget* table = new QTableWidget(this);
	table->setObjectName(objectName.c_str());
	table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
	table->setShowGrid(true);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	table->verticalHeader()->hide();
	table->setAlternatingRowColors(true);
	table->horizontalScrollBar()->setDisabled(true);
	table->horizontalScrollBar()->hide();
	table->setAutoScroll(false);

	table->setColumnCount(2);
	table->setHorizontalHeaderItem(0, new QTableWidgetItem("Command"));
	table->setHorizontalHeaderItem(1, new QTableWidgetItem("Shortcut"));

	table->setFixedWidth(610);

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGenerelShortcutsTable()
{
	QTableWidget* table = createTableWidget("table_general");

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

	table->setFixedHeight(480); // there doesn't seem to be an automatic method, so fuck you basically...

	return table;
}

QTableWidget* QtKeyboardShortcuts::createCodeViewShortcutsTable()
{
	QTableWidget* table = createTableWidget("table_code");

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

	table->setFixedHeight(90); // there doesn't seem to be an automatic method, so fuck you basically...

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGraphViewShortcutsTable()
{
	QTableWidget* table = createTableWidget("table_graph");

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

	table->setFixedHeight(240); // there doesn't seem to be an automatic method, so fuck you basically...

	return table;
}
