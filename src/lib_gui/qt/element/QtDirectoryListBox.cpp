#include "qt/element/QtDirectoryListBox.h"

#include <QBoxLayout>
#include <QMimeData>
#include <QScrollBar>
#include <QTimer>

#include "qt/element/QtIconButton.h"
#include "qt/utility/utilityQt.h"
#include "qt/utility/QtFileDialog.h"
#include "qt/window/QtTextEditDialog.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"

QtListItemWidget::QtListItemWidget(QtDirectoryListBox* list, QListWidgetItem* item, QWidget *parent)
	: QWidget(parent)
	, m_list(list)
	, m_item(item)
	, m_readOnly(false)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(3);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	m_data = new QtLineEdit(this);
	m_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_data->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_data->setObjectName("field");
    m_data->setAcceptDrops(false);

	m_button = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"window/dots.png"),
		ResourcePaths::getGuiPath().concatenate(L"window/dots_hover.png")
	);
	m_button->setIconSize(QSize(16, 16));
	m_button->setObjectName("dotsButton");

	layout->addWidget(m_data);
	layout->addWidget(m_button);

	if (list->isForStrings())
	{
		m_button->hide();
	}

	setLayout(layout);

	connect(m_button, &QPushButton::clicked, this, &QtListItemWidget::handleButtonPress);
	connect(m_data, &QtLineEdit::focus, this, &QtListItemWidget::handleFocus);
}

QString QtListItemWidget::getText()
{
	return m_data->text();
}

void QtListItemWidget::setText(QString text)
{
	FilePath relativeRoot = m_list->getRelativeRootDirectory();
	if (!relativeRoot.empty())
	{
		const FilePath path(text.toStdWString());
		const FilePath relPath = path.getRelativeTo(relativeRoot);
		if (relPath.wstr().size() < path.wstr().size())
		{
			text = QString::fromStdWString(relPath.wstr());
		}
	}

	m_data->setText(text);
}

bool QtListItemWidget::readOnly() const
{
	return m_readOnly;
}

void QtListItemWidget::setReadOnly(bool readOnly)
{
	m_readOnly = readOnly;

	if (readOnly)
	{
		m_item->setFlags(m_item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	}
	else
	{
		m_item->setFlags(m_item->flags() | ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	}

	m_data->setReadOnly(readOnly);
	m_data->setEnabled(!readOnly);
	m_button->setEnabled(!readOnly);
}

void QtListItemWidget::setFocus()
{
    m_data->setFocus(Qt::OtherFocusReason);
}

void QtListItemWidget::handleButtonPress()
{
	FilePath path(m_data->text().toStdWString());
	const FilePath relativeRoot = m_list->getRelativeRootDirectory();
	if (!path.empty() && !path.isAbsolute() && !relativeRoot.empty())
	{
		path = relativeRoot.getConcatenated(path);
	}

	QStringList list = QtFileDialog::getFileNamesAndDirectories(this, path);
	if (!list.isEmpty())
	{
		setText(list.at(0));
	}

	for (int i = 1; i < list.size(); i++)
	{
		m_list->addListBoxItemWithText(list.at(i));
	}

	handleFocus();
}

void QtListItemWidget::handleFocus()
{
	m_list->selectItem(m_item);
}


QtDirectoryListBox::QtDirectoryListBox(QWidget *parent, const QString& listName, bool forStrings)
	: QFrame(parent)
	, m_listName(listName)
	, m_forStrings(forStrings)
{
	QBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 6, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	m_list = new QListWidget(this);
	m_list->setObjectName("list");
	m_list->setAttribute(Qt::WA_MacShowFocusRect, 0);

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/listbox.css")).c_str());
	layout->addWidget(m_list, 5);

	QWidget* buttonContainer = new QWidget(this);
	buttonContainer->setObjectName("bar");

	QHBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(8, 4, 8, 2);
	innerLayout->setSpacing(0);

	m_addButton = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"window/plus.png"),
		ResourcePaths::getGuiPath().concatenate(L"window/plus_hover.png")
	);
	m_addButton->setIconSize(QSize(16, 16));
	m_addButton->setObjectName("plusButton");
	m_addButton->setToolTip("add line");
	innerLayout->addWidget(m_addButton);

	m_removeButton = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"window/minus.png"),
		ResourcePaths::getGuiPath().concatenate(L"window/minus_hover.png")
	);
	m_removeButton->setIconSize(QSize(16, 16));
	m_removeButton->setObjectName("minusButton");
	m_removeButton->setToolTip("remove line");
	innerLayout->addWidget(m_removeButton);

	innerLayout->addStretch();

	QLabel* dropInfoText = new QLabel("Drop Files & Folders");
	dropInfoText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	dropInfoText->setObjectName("dropInfo");
	dropInfoText->setAlignment(Qt::AlignRight);
	innerLayout->addWidget(dropInfoText);

	QPushButton* editButton = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"code_view/images/edit.png"),
		FilePath()
	);
	editButton->setIconSize(QSize(16, 16));
	editButton->setObjectName("editButton");
	editButton->setToolTip("edit plain text");
	innerLayout->addWidget(editButton);

	if (isForStrings())
	{
		dropInfoText->hide();
	}

	buttonContainer->setLayout(innerLayout);
	layout->addWidget(buttonContainer, 0);
	setLayout(layout);

	connect(m_addButton, &QPushButton::clicked, this, &QtDirectoryListBox::addListBoxItem);
	connect(m_removeButton, &QPushButton::clicked, this, &QtDirectoryListBox::removeListBoxItem);
	connect(editButton, &QPushButton::clicked, this, &QtDirectoryListBox::showEditDialog);

	setAcceptDrops(true);
	setMaximumHeight(160);
}

void QtDirectoryListBox::clear()
{
	m_list->clear();
}

bool QtDirectoryListBox::event(QEvent* event)
{
	// Prevent nested ScrollAreas to scroll at the same time;
	if (event->type() == QEvent::Wheel)
	{
		QRect rect = m_list->viewport()->rect();
		QPoint pos = m_list->mapFromGlobal( dynamic_cast<QWheelEvent*>(event)->globalPos());
		QScrollBar* bar = m_list->verticalScrollBar();

		if (bar->minimum() != bar->maximum() && rect.contains(pos))
		{
			bool down = dynamic_cast<QWheelEvent*>(event)->angleDelta().y() < 0;

			if ((down && bar->value() != bar->maximum()) || (!down && bar->value() != bar->minimum()))
			{
				return true;
			}
		}
	}

	return QFrame::event(event);
}

void QtDirectoryListBox::dropEvent(QDropEvent *event)
{
	foreach(QUrl url, event->mimeData()->urls())
	{
		QtListItemWidget* widget = addListBoxItem();
		widget->setText(url.toLocalFile());
	}
}

std::vector<FilePath> QtDirectoryListBox::getList()
{
	std::vector<FilePath> list;
	for (const std::wstring& s : getStringList())
	{
		list.push_back(FilePath(s));
	}
	return list;
}

void QtDirectoryListBox::setList(const std::vector<FilePath>& list, bool readOnly)
{
	std::vector<std::wstring> strList;
	for (const FilePath& path : list)
	{
		strList.push_back(path.wstr());
	}
	setStringList(strList, readOnly);
}

std::vector<std::wstring> QtDirectoryListBox::getStringList()
{
	std::vector<std::wstring> list;
	for (int i = 0; i < m_list->count(); ++i)
	{
		QtListItemWidget* widget = dynamic_cast<QtListItemWidget*>(m_list->itemWidget(m_list->item(i)));
		list.push_back(widget->getText().toStdWString());
	}
	return list;
}

void QtDirectoryListBox::setStringList(const std::vector<std::wstring>& list, bool readOnly)
{
	clear();

	FilePath root = m_relativeRootDirectory;
	m_relativeRootDirectory = FilePath();

	for (const std::wstring& str : list)
	{
		QtListItemWidget* itemWidget = addListBoxItemWithText(QString::fromStdWString(str));
		itemWidget->setReadOnly(readOnly);
	}

	m_list->scrollToTop();
	m_relativeRootDirectory = root;
}

QtListItemWidget* QtDirectoryListBox::addListBoxItemWithText(const QString& text)
{
	QtListItemWidget* widget = addListBoxItem();
	widget->setText(text);
	return widget;
}

void QtDirectoryListBox::selectItem(QListWidgetItem* item)
{
	for (int i = 0; i < m_list->count(); i++)
	{
		m_list->item(i)->setSelected(false);
	}

	item->setSelected(true);
}

bool QtDirectoryListBox::isForStrings() const
{
	return m_forStrings;
}

const FilePath& QtDirectoryListBox::getRelativeRootDirectory() const
{
	return m_relativeRootDirectory;
}

void QtDirectoryListBox::setRelativeRootDirectory(const FilePath& dir)
{
	m_relativeRootDirectory = dir;
}

QtListItemWidget* QtDirectoryListBox::addListBoxItem()
{
	QListWidgetItem *item = new QListWidgetItem(m_list);
	m_list->addItem(item);

	QtListItemWidget* widget = new QtListItemWidget(this, item);
	m_list->setItemWidget(item, widget);

	m_list->scrollToItem(item);
	widget->setFocus();

	return widget;
}

void QtDirectoryListBox::removeListBoxItem()
{
	if (!m_list->selectedItems().size())
	{
		return;
	}

	int rowIndex = m_list->row(m_list->selectedItems().first());

	qDeleteAll(m_list->selectedItems());

	if (rowIndex == m_list->count())
	{
		rowIndex -= 1;
	}

	if (rowIndex >= 0)
	{
		m_list->setCurrentRow(rowIndex);
	}
}

void QtDirectoryListBox::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void QtDirectoryListBox::showEditDialog()
{
	if (!m_editDialog)
	{
		m_editDialog = std::make_shared<QtTextEditDialog>(m_listName, "Edit the list in plain text. Each line is one item.");
		m_editDialog->setup();

		std::vector<std::wstring> list;
		for (int i = 0; i < m_list->count(); ++i)
		{
			QtListItemWidget* widget = dynamic_cast<QtListItemWidget*>(m_list->itemWidget(m_list->item(i)));
			if (!widget->readOnly())
			{
				list.push_back(widget->getText().toStdWString());
			}
		}

		m_editDialog->setText(utility::join(list, L"\n"));

		connect(m_editDialog.get(), &QtTextEditDialog::canceled, this, &QtDirectoryListBox::canceledEditDialog);
		connect(m_editDialog.get(), &QtTextEditDialog::finished, this, &QtDirectoryListBox::savedEditDialog);
	}

	m_editDialog->showWindow();
	m_editDialog->raise();
}

void QtDirectoryListBox::canceledEditDialog()
{
	m_editDialog->hide();
	m_editDialog.reset();

	window()->raise();
}

void QtDirectoryListBox::savedEditDialog()
{
	std::vector<std::string> readOnlyLines;
	for (int i = 0; i < m_list->count(); ++i)
	{
		QtListItemWidget* widget = dynamic_cast<QtListItemWidget*>(m_list->itemWidget(m_list->item(i)));
		if (widget->readOnly())
		{
			readOnlyLines.push_back(widget->getText().toStdString());
		}
	}

	std::vector<std::wstring> lines = utility::splitToVector(m_editDialog->getText(), L"\n");
	for (size_t i = 0; i < lines.size(); i++)
	{
		lines[i] = utility::trim(lines[i]);

		if (lines[i].empty())
		{
			lines.erase(lines.begin() + i);
			i--;
		}
	}

	clear();

	FilePath root = m_relativeRootDirectory;
	m_relativeRootDirectory = FilePath();

	for (const std::string& str : readOnlyLines)
	{
		QtListItemWidget* itemWidget = addListBoxItemWithText(QString::fromStdString(str));
		itemWidget->setReadOnly(true);
	}

	for (const std::wstring& line : lines)
	{
		QtListItemWidget* itemWidget = addListBoxItemWithText(QString::fromStdWString(line));
		itemWidget->setReadOnly(false);
	}

	m_relativeRootDirectory = root;

	canceledEditDialog();
}
