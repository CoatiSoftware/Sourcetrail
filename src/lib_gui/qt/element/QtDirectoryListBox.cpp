#include "qt/element/QtDirectoryListBox.h"

#include <QBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include <QScrollBar>
#include <QSysInfo>
#include <QTreeView>

#include "utility/ResourcePaths.h"

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
	m_data->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_data->setObjectName("field");

	m_button = new QPushButton("");
	m_button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_button->setObjectName("dotsButton");

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

	if (QSysInfo::macVersion() == QSysInfo::MV_None)
	{
		dialog.setFileMode(QFileDialog::Directory);
	}

	if (m_data->text().size())
	{
		dialog.setDirectory(m_data->text());
	}

	QListView *l = dialog.findChild<QListView*>("listView");
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
	m_list->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "window/listbox.css").c_str());
	layout->addWidget(m_list);

	QWidget* buttonContainer = new QWidget(this);
	buttonContainer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	buttonContainer->setObjectName("bar");

	QHBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(8, 4, 8, 3);
	innerLayout->setSpacing(0);

	m_addButton = new QPushButton("", this);
	m_addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_addButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_addButton->setObjectName("plusButton");
	innerLayout->addWidget(m_addButton);

	m_removeButton = new QPushButton("", this);
	m_removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_removeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_removeButton->setObjectName("minusButton");
	innerLayout->addWidget(m_removeButton);

	QLabel* dropInfoText = new QLabel("Drop Files & Folders");
	dropInfoText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	dropInfoText->setObjectName("dropInfo");
	dropInfoText->setAlignment(Qt::AlignRight);
	innerLayout->addWidget(dropInfoText);

	layout->addStretch();

	buttonContainer->setLayout(innerLayout);
	layout->addWidget(buttonContainer);
	setLayout(layout);

	connect(m_addButton, SIGNAL(clicked()), this, SLOT(addListBoxItem()));
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeListBoxItem()));

	setAcceptDrops(true);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Minimum);
	setMaximumHeight(200);
	resize();
}

QSize QtDirectoryListBox::sizeHint() const
{
	return QSize(QFrame::sizeHint().width(), 100);
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
	int height = m_list->height() - m_list->viewport()->height();

	if (m_list->count() > 0)
	{
		height += (m_list->itemWidget(m_list->item(0))->height() + 1) * m_list->count() + 7;
	}

	if (height < 0)
	{
		height = 0;
	}

	m_list->setMaximumHeight(height);
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

	resize();

	if (rowIndex >= 0)
	{
		m_list->setCurrentRow(rowIndex);
	}
}

void QtDirectoryListBox::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}
