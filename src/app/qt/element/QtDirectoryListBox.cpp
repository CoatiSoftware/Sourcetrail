#include "qt/element/QtDirectoryListBox.h"

#include <QBoxLayout>
#include <QTreeView>
#include <QFileDialog>
#include <QLabel>
#include <QMimeData>

#include "qt/utility/utilityQt.h"

QtDirectoryListBox::QtDirectoryListBox(QWidget *parent)
		:QFrame(parent)
{
	setObjectName("QtListBox");
	setMinimumHeight(150);
	QBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setAlignment(Qt::AlignTop);
	m_list = new QListWidget(this);

	setStyleSheet(utility::getStyleSheet("data/gui/settingwindows/listbox.css").c_str());
	layout->addWidget(m_list);

	QWidget* buttonContainer = new QWidget(this);
	QBoxLayout* innerLayout = new QHBoxLayout();
	innerLayout->setContentsMargins(0,0,0,0);

	m_addButton = new QPushButton("+",this);
	m_addButton->resize(20,20);
	m_addButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_addButton->setObjectName("roundedButton");
	innerLayout->addWidget(m_addButton);

	m_removeButton = new QPushButton("-",this);
	m_removeButton->resize(20,20);
	m_removeButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_removeButton->setObjectName("roundedButton");
	innerLayout->addWidget(m_removeButton);

	QLabel* dropInfoText = new QLabel("You can drop Folders here");
	dropInfoText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	dropInfoText->setObjectName("dropInfo");
	dropInfoText->setAlignment(Qt::AlignRight);
	innerLayout->addWidget(dropInfoText);
	buttonContainer->setLayout(innerLayout);
	layout->addWidget(buttonContainer);
	setLayout(layout);

	connect(m_addButton, SIGNAL(clicked()), this, SLOT(addListBoxItem()));
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeListBoxItem()));

	setAcceptDrops(true);
}

void QtDirectoryListBox::dropEvent(QDropEvent *event)
{
	QFileInfo fileInfo;
	foreach(QUrl url, event->mimeData()->urls())
	{
		fileInfo.setFile(url.toLocalFile());
		if(fileInfo.isDir())
		{
			QListWidgetItem *item = new QListWidgetItem(m_list);
			m_list->addItem(item);
			item->setText(url.toLocalFile());
		}
	}
}

std::vector<FilePath> QtDirectoryListBox::getList()
{
	std::vector<FilePath> list;
	for( int i = 0; i < m_list->count(); ++i)
	{
		QListWidgetItem* item = m_list->item(i);
		list.push_back(item->text().toStdString());
	}

	return list;
}

void QtDirectoryListBox::addListBoxItem()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	dialog.setOption(QFileDialog::DontUseNativeDialog,true);

	// enable multiselection of directories
	QListView *l = dialog.findChild<QListView*>("listView");
    if (l)
    {
        l->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView *t = dialog.findChild<QTreeView*>();
 	if (t)
 	{
       t->setSelectionMode(QAbstractItemView::MultiSelection);
   	}

	if(dialog.exec())
	{
		QStringList list = dialog.selectedFiles();
		for(int i = 0; i < list.size(); i++)
		{
			QListWidgetItem *item = new QListWidgetItem(m_list);
			m_list->addItem(item);
			item->setText(list.at(i));
		}
	}
}

void QtDirectoryListBox::removeListBoxItem()
{
	qDeleteAll(m_list->selectedItems());
}

void QtDirectoryListBox::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}
