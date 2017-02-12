#include "QtBookmarkBrowser.h"

#include "qt/element/QtBookmark.h"
#include "qt/element/QtBookmarkCategory.h"
#include <QHeaderView>

#include "data/bookmark/Bookmark.h"

#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtBookmarkBrowser::QtBookmarkBrowser(QWidget* parent)
	: QtWindow(parent)
	, m_headerBackground(NULL)
{
}

QtBookmarkBrowser::~QtBookmarkBrowser()
{
}

void QtBookmarkBrowser::setupBookmarkBrowser()
{
	m_layout = new QHBoxLayout();
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setAlignment(Qt::AlignTop);
	setLayout(m_layout);

	m_headerBackground = new QWidget(m_window);
	m_headerBackground->setObjectName("header_background");
	m_headerBackground->setGeometry(0, 0, 0, 0);
	m_headerBackground->show();
	m_headerBackground->lower();

	m_headerLayout = new QVBoxLayout(this);
	m_headerLayout->setObjectName("header");
	m_headerLayout->setSpacing(0);
	m_headerLayout->setContentsMargins(0, 0, 0, 0);
	m_headerLayout->setAlignment(Qt::AlignTop);
	m_layout->addLayout(m_headerLayout);

	m_bodyLayout = new QVBoxLayout(this);
	m_bodyLayout->setObjectName("body");
	m_bodyLayout->setSpacing(0);
	m_bodyLayout->setContentsMargins(0, 0, 0, 0);
	m_bodyLayout->setAlignment(Qt::AlignLeft);
	m_layout->addLayout(m_bodyLayout);

	m_title = new QLabel(this);
	m_title->setObjectName("title");
	m_title->setText("Bookmarks");
	m_headerLayout->addWidget(m_title);

	m_filterLabel = new QLabel(this);
	m_filterLabel->setObjectName("filter_label");
	m_filterLabel->setText("Show:");
	m_headerLayout->addWidget(m_filterLabel);

	m_filterComboBox = new QComboBox(this);
	m_filterComboBox->addItem("All");
	m_filterComboBox->addItem("Nodes");
	m_filterComboBox->addItem("Edges");
	m_filterComboBox->setObjectName("filter_box");
	m_headerLayout->addWidget(m_filterComboBox);

	connect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(filterBoxChanged(const QString&)));

	m_orderLabel = new QLabel(this);
	m_orderLabel->setObjectName("order_label");
	m_orderLabel->setText("Sort by:");
	m_headerLayout->addWidget(m_orderLabel);

	m_orderNames.push_back("Date des.");
	m_orderNames.push_back("Date asc.");
	m_orderNames.push_back("Name des.");
	m_orderNames.push_back("Name asc.");
	m_currentOrderIndex = 0;

	m_orderComboBox = new QComboBox(this);
	// m_orderComboBox->setToolTip("Select Bookmark Order");
	m_orderComboBox->addItem(m_orderNames[0].c_str());
	m_orderComboBox->addItem(m_orderNames[1].c_str());
	m_orderComboBox->addItem(m_orderNames[2].c_str());
	m_orderComboBox->addItem(m_orderNames[3].c_str());
	m_orderComboBox->setObjectName("order_box");
	m_headerLayout->addWidget(m_orderComboBox);

	connect(m_orderComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(orderBoxChanged(const QString&)));


	m_headerLayout->addWidget(m_orderComboBox);

	m_bookmarkTree = new QTreeWidget();
	m_bookmarkTree->setObjectName("bookmark_tree");
	m_bookmarkTree->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	m_bookmarkTree->header()->close();
	m_bookmarkTree->setAlternatingRowColors(true);
	m_bookmarkTree->setIndentation(0);
	m_bookmarkTree->setHeaderLabel("Bookmarks");

	connect(m_bookmarkTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
	connect(m_bookmarkTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(itemCollapsed(QTreeWidgetItem*)));

	// m_bookmarkTree->icon

	m_bodyLayout->addWidget(m_bookmarkTree);

	m_closeButton = new QPushButton(this);
	m_closeButton->setObjectName("close_button");
	m_closeButton->setToolTip("Close Window");
	m_closeButton->setText("Close");
	m_closeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_bodyLayout->addWidget(m_closeButton);
	m_bodyLayout->setAlignment(m_closeButton, Qt::AlignRight);

	connect(m_closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));

	setFixedSize(QSize(790, 600));
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "bookmark_view/bookmark_view.css").c_str());

	m_headerBackground->setGeometry(0, 0, 247, size().height()-20);
}

void QtBookmarkBrowser::setBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_bookmarkTree->clear();
	// m_bookmarkTree->adjustSize();

	for (unsigned int i = 0; i < bookmarks.size(); i++)
	{
		QtBookmark* bookmark = new QtBookmark();
		bookmark->setName(bookmarks[i]->getDisplayName());
		bookmark->setBookmark(bookmarks[i]);

		QTreeWidgetItem* top = findOrCreateTreeCategory(bookmarks[i]->getCategory().getName(), bookmarks[i]->getCategory().getId());
		QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(top);
		// treeWidgetItem->setText(0, bookmarks[i]->getDisplayName().c_str());
		treeWidgetItem->setSizeHint(0, bookmark->minimumSizeHint());
		m_bookmarkTree->setItemWidget(treeWidgetItem, 0, bookmark);
		top->addChild(treeWidgetItem);

		bookmark->setTreeWidgetItem(top);

		top->setExpanded(true);
	}
}

void QtBookmarkBrowser::closeButtonClicked()
{
	try
	{
		close();
	}
	catch (std::exception& e)
	{
		std::string message = e.what();

		LOG_ERROR(message);
	}
}

void QtBookmarkBrowser::filterBoxChanged(const QString& text)
{
	MessageDisplayBookmarks(getSelectedFilter(), getSelectedOrder()).dispatch();
}

void QtBookmarkBrowser::orderBoxChanged(const QString& text)
{
	MessageDisplayBookmarks(getSelectedFilter(), getSelectedOrder()).dispatch();
}

void QtBookmarkBrowser::itemExpanded(QTreeWidgetItem* item)
{
	QWidget* widget = m_bookmarkTree->itemWidget(item, 0);

	QtBookmarkCategory* category = dynamic_cast<QtBookmarkCategory*>(widget);

	if (category != NULL)
	{
		category->updateArrow();
	}
}

void QtBookmarkBrowser::itemCollapsed(QTreeWidgetItem* item)
{
	QWidget* widget = m_bookmarkTree->itemWidget(item, 0);

	QtBookmarkCategory* category = dynamic_cast<QtBookmarkCategory*>(widget);

	if (category != NULL)
	{
		category->updateArrow();
	}
}

void QtBookmarkBrowser::handleMessage(MessageDeleteBookmark* message)
{
	MessageDisplayBookmarks(getSelectedFilter(), getSelectedOrder()).dispatch();
}

void QtBookmarkBrowser::handleMessage(MessageEditBookmark* message)
{
	MessageDisplayBookmarks(getSelectedFilter(), getSelectedOrder()).dispatch();
}

MessageDisplayBookmarks::BookmarkFilter QtBookmarkBrowser::getSelectedFilter()
{
	std::string text = m_filterComboBox->currentText().toStdString();

	if (text == "Nodes")
	{
		return MessageDisplayBookmarks::BookmarkFilter::NODES;
	}
	else if (text == "Edges")
	{
		return MessageDisplayBookmarks::BookmarkFilter::EDGES;
	}

	return MessageDisplayBookmarks::BookmarkFilter::ALL;
}

MessageDisplayBookmarks::BookmarkOrder QtBookmarkBrowser::getSelectedOrder()
{
	std::string orderString = m_orderComboBox->currentText().toStdString(); // m_orderButton->text().toStdString();

	if (orderString == m_orderNames[0])
	{
		return MessageDisplayBookmarks::BookmarkOrder::DATE_DESCENDING;
	}
	else if (orderString == m_orderNames[1])
	{
		return MessageDisplayBookmarks::BookmarkOrder::DATE_ASCENDING;
	}
	else if (orderString == m_orderNames[2])
	{
		return MessageDisplayBookmarks::BookmarkOrder::NAME_DESCENDING;
	}
	else if (orderString == m_orderNames[3])
	{
		return MessageDisplayBookmarks::BookmarkOrder::NAME_ASCENDING;
	}
	else
	{
		return MessageDisplayBookmarks::BookmarkOrder::NONE;
	}
}

QTreeWidgetItem* QtBookmarkBrowser::findOrCreateTreeCategory(const std::string& name, const Id id)
{
	for (int i = 0; i < m_bookmarkTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_bookmarkTree->topLevelItem(i);

		if (item->whatsThis(0).toStdString() == name)
		{
			return item;
		}
	}

	QtBookmarkCategory* category = new QtBookmarkCategory();
	if (name.length() > 0)
	{
		category->setName(name);
	}
	else
	{
		category->setName("No Category");
	}
	category->setId(id);

	QTreeWidgetItem* newItem = new QTreeWidgetItem(m_bookmarkTree);
	newItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	newItem->setSizeHint(0, category->minimumSizeHint());
	newItem->setWhatsThis(0, name.c_str());
	category->setTreeWidgetItem(newItem);

	m_bookmarkTree->setItemWidget(newItem, 0, category);

	m_bookmarkTree->addTopLevelItem(newItem);

	return newItem;
}
