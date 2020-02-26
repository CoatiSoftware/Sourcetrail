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
	: name(name)
	, shortcut(shortcut)
{}

QtKeyboardShortcuts::Shortcut QtKeyboardShortcuts::Shortcut::defaultOrMac(
	const QString& name, const QString& defaultShortcut, const QString& macShortcut)
{
#if defined(Q_OS_MAC)
	return { name, macShortcut };
#else
	return { name, defaultShortcut };
#endif
}

QtKeyboardShortcuts::Shortcut QtKeyboardShortcuts::Shortcut::winMacOrLinux(
	const QString& name, const QString& winShortcut, const QString& macShortcut, const QString& linuxShortcut)
{
#if defined(Q_OS_WIN32)
	return { name, winShortcut };
#elif defined(Q_OS_MAC)
	return { name, macShortcut };
#else
	return { name, linuxShortcut };
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
	table->setRowCount(shortcuts.size());

	for (size_t i = 0; i < shortcuts.size(); ++i)
	{
		table->setItem(i, 0, new QTableWidgetItem(shortcuts[i].name));
		table->setItem(i, 1, new QTableWidgetItem(shortcuts[i].shortcut));
	}

	table->updateSize();
}

QTableWidget* QtKeyboardShortcuts::createGenerelShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_general");

	addShortcuts(table,
		{
			Shortcut::defaultOrMac(QStringLiteral("Larger Font"), QStringLiteral("Ctrl + +"), QStringLiteral("Cmd + +")),
			Shortcut::defaultOrMac(QStringLiteral("Smaller Font"), QStringLiteral("Ctrl + -"), QStringLiteral("Cmd + -")),
			Shortcut::defaultOrMac(QStringLiteral("Reset Font Size"), QStringLiteral("Ctrl + 0"), QStringLiteral("Cmd + 0")),
			Shortcut::defaultOrMac(QStringLiteral("Back"), QStringLiteral("Ctrl + Z | Backspace"), QStringLiteral("Cmd + Z | Backspace")),
			Shortcut::defaultOrMac(QStringLiteral("Forward"), QStringLiteral("Ctrl + Shift + Z"), QStringLiteral("Cmd + Shift + Z")),
			Shortcut::defaultOrMac(QStringLiteral("Refresh"), QStringLiteral("F5"), QStringLiteral("Cmd + R")),
			Shortcut::defaultOrMac(QStringLiteral("Full Refresh"), QStringLiteral("Shift + F5"), QStringLiteral("Cmd + Shift + R")),
			Shortcut::defaultOrMac(QStringLiteral("Find Symbol"), QStringLiteral("Ctrl + F"), QStringLiteral("Cmd + F")),
			Shortcut::defaultOrMac(QStringLiteral("Find Text"), QStringLiteral("Ctrl + Shift + F"), QStringLiteral("Cmd + Shift + F")),
			Shortcut::defaultOrMac(QStringLiteral("Find On-Screen"), QStringLiteral("Ctrl + D | /"), QStringLiteral("Cmd + D | /")),
			Shortcut::defaultOrMac(QStringLiteral("New Project"), QStringLiteral("Ctrl + N"), QStringLiteral("Cmd + N")),
			Shortcut::defaultOrMac(QStringLiteral("Open Project"), QStringLiteral("Ctrl + O"), QStringLiteral("Cmd + O")),
			Shortcut::winMacOrLinux(QStringLiteral("Close Window"), QStringLiteral("Alt + F4"), QStringLiteral("Cmd + W"), QStringLiteral("Ctrl + W")),
			Shortcut::defaultOrMac(QStringLiteral("Hide Window"), QStringLiteral(""), QStringLiteral("Cmd + H")),
			Shortcut::defaultOrMac(QStringLiteral("To Overview"), QStringLiteral("Ctrl + Home"), QStringLiteral("Cmd + Home | Cmd + Up")),
			Shortcut::defaultOrMac(QStringLiteral("Preferences"), QStringLiteral("Ctrl + ,"), QStringLiteral("Cmd + ,")),
			Shortcut::defaultOrMac(QStringLiteral("Bookmark Active Symbol"), QStringLiteral("Ctrl + S"), QStringLiteral("Cmd + S")),
			Shortcut::defaultOrMac(QStringLiteral("Bookmark Manager"), QStringLiteral("Ctrl + B"), QStringLiteral("Cmd + B"))
		}
	);

	return table;
}

QTableWidget* QtKeyboardShortcuts::createCodeViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_code");

	addShortcuts(table,
		{
			Shortcut::defaultOrMac(QStringLiteral("Next Reference"), QStringLiteral("Ctrl + G"), QStringLiteral("Cmd + G")),
			Shortcut::defaultOrMac(QStringLiteral("Previous Reference"), QStringLiteral("Ctrl + Shift + G"), QStringLiteral("Cmd + Shift + G")),
			Shortcut::defaultOrMac(QStringLiteral("Next Local Reference"), QStringLiteral("Ctrl + L"), QStringLiteral("Cmd + L")),
			Shortcut::defaultOrMac(QStringLiteral("Previous Local Reference"), QStringLiteral("Ctrl + Shift + L"), QStringLiteral("Cmd + Shift + L"))
		}
	);

	return table;
}

QTableWidget* QtKeyboardShortcuts::createGraphViewShortcutsTable()
{
	QtShortcutTable* table = createTableWidget("table_graph");

	addShortcuts(table,
		{
			Shortcut(QStringLiteral("Pan left"), QStringLiteral("A")),
			Shortcut(QStringLiteral("Pan right"), QStringLiteral("D")),
			Shortcut(QStringLiteral("Pan up"), QStringLiteral("W")),
			Shortcut(QStringLiteral("Pan down"), QStringLiteral("S")),
			Shortcut::defaultOrMac(QStringLiteral("Zoom in"), QStringLiteral("Shift + W | Ctrl + Mousewheel up"), QStringLiteral("Shift + W | Cmd + Mousewheel up")),
			Shortcut::defaultOrMac(QStringLiteral("Zoom out"), QStringLiteral("Shift + S | Ctrl + Mousewheel down"), QStringLiteral("Shift + S | Cmd + Mousewheel down")),
			Shortcut(QStringLiteral("Reset Zoom"), QStringLiteral("0")),
			Shortcut::defaultOrMac(QStringLiteral("Custom Trail Dialog"), QStringLiteral("Ctrl + U"), QStringLiteral("Cmd + U"))
		}
	);

	return table;
}
