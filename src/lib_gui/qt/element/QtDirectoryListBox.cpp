#include "qt/element/QtDirectoryListBox.h"

#include <QBoxLayout>
#include <QTreeView>
#include <QFileDialog>
#include <QMimeData>

#include "qt/utility/utilityQt.h"

QtListItemWidget::QtListItemWidget(QtDirectoryListBox* list, QListWidgetItem* item, QWidget *parent)
	: QWidget(parent)
	, m_list(list)
	, m_item(item)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(3);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	m_data = new QtLineEdit(this);
	m_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_data->setObjectName("field");

	m_button = new QPushButton("...");
	m_button->setObjectName("button");

	layout->addWidget(m_data);
	layout->addWidget(m_button);

	setLayout(layout);

	connect(m_button, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
	connect(m_data, SIGNAL(focus()), this, SLOT(handleFocus()));
}

QString QtListItemWidget::getText()
{
	return m_data->text();
}

void QtListItemWidget::setText(QString text)
{
	m_data->setText(text);
}

void QtListItemWidget::setFocus()
{
	m_data->setFocus(Qt::OtherFocusReason);
}

void QtListItemWidget::handleButtonPress()
{
	QFileDialog dialog(this);
	QListView *l = dialog.findChild<QListView*>("listView");

	dialog.setFileMode(QFileDialog::Directory);

	if (l)
    {
        l->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    QTreeView *t = dialog.findChild<QTreeView*>();
	if (t)
	{
       t->setSelectionMode(QAbstractItemView::SingleSelection);
	}
	if (dialog.exec())
	{
		QStringList list = dialog.selectedFiles();
		for (int i = 0; i < list.size(); i++)
		{
			setText(list.at(i));
		}
	}
	handleFocus();
}

void QtListItemWidget::handleFocus()
{
	m_list->selectItem(m_item);
}


QtDirectoryListBox::QtDirectoryListBox(QWidget *parent)
	:QFrame(parent)
{
	QBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 6, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	m_list = new QListWidget(this);
	m_list->setObjectName("list");
	m_list->setAttribute(Qt::WA_MacShowFocusRect, 0);

	setStyleSheet(utility::getStyleSheet("data/gui/setting_window/listbox.css").c_str());
	layout->addWidget(m_list);

	QWidget* buttonContainer = new QWidget(this);
	buttonContainer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	buttonContainer->setObjectName("bar");

	QHBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(8, 4, 8, 3);
	innerLayout->setSpacing(0);

	m_addButton = new QPushButton(QIcon("data/gui/setting_window/plus.png"), "", this);
	m_addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_addButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_addButton->setObjectName("roundedButton");
	innerLayout->addWidget(m_addButton);

	m_removeButton = new QPushButton(QIcon("data/gui/setting_window/minus.png"), "", this);
	m_removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_removeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_removeButton->setObjectName("roundedButton");
	innerLayout->addWidget(m_removeButton);

	QLabel* dropInfoText = new QLabel("Drop Files & Folders");
	dropInfoText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	dropInfoText->setObjectName("dropInfo");
	dropInfoText->setAlignment(Qt::AlignRight);
	innerLayout->addWidget(dropInfoText);

	buttonContainer->setLayout(innerLayout);
	layout->addWidget(buttonContainer);
	setLayout(layout);

	connect(m_addButton, SIGNAL(clicked()), this, SLOT(addListBoxItem()));
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeListBoxItem()));

	setAcceptDrops(true);
	resize();
}

void QtDirectoryListBox::clear()
{
	m_list->clear();
}

void QtDirectoryListBox::dropEvent(QDropEvent *event)
{
	QFileInfo fileInfo;
	foreach(QUrl url, event->mimeData()->urls())
	{
		QtListItemWidget* widget = addListBoxItem();
		widget->setText(url.toLocalFile());
	}
}

std::vector<FilePath> QtDirectoryListBox::getList()
{
	std::vector<FilePath> list;
	for (int i = 0; i < m_list->count(); ++i)
	{
		QtListItemWidget* widget = dynamic_cast<QtListItemWidget*>(m_list->itemWidget(m_list->item(i)));
		list.push_back(widget->getText().toStdString());
	}
	return list;
}

void QtDirectoryListBox::setList(const std::vector<FilePath>& list)
{
	m_list->clear();

	for (const FilePath& path : list)
	{
		QtListItemWidget* widget = addListBoxItem();
		widget->setText(QString::fromStdString(path.str()));
	}
}

void QtDirectoryListBox::selectItem(QListWidgetItem* item)
{
	for (int i = 0; i < m_list->count(); i++)
	{
		m_list->item(i)->setSelected(false);
	}

	item->setSelected(true);
}

void QtDirectoryListBox::resize()
{
	int height = 25;

	if (m_list->count() > 0)
	{
		height += (m_list->itemWidget(m_list->item(0))->height() + 1) * m_list->count() + 8;
	}

	if (height < 150)
	{
		height = 150;
	}

	setMinimumHeight(height);
}

QtListItemWidget* QtDirectoryListBox::addListBoxItem()
{
	QListWidgetItem *item = new QListWidgetItem(m_list);
	m_list->addItem(item);

	QtListItemWidget* widget = new QtListItemWidget(this, item);
	m_list->setItemWidget(item, widget);

	resize();

	widget->setFocus();

	return widget;
}

void QtDirectoryListBox::removeListBoxItem()
{
	qDeleteAll(m_list->selectedItems());
	resize();
}

void QtDirectoryListBox::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}
