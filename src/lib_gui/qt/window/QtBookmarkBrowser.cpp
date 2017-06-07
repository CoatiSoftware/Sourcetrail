#include "QtBookmarkBrowser.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "data/bookmark/Bookmark.h"
#include "qt/element/QtBookmark.h"
#include "qt/element/QtBookmarkCategory.h"
#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtBookmarkBrowser::QtBookmarkBrowser(QWidget* parent)
    : QtWindow(false, parent)
{
}

QtBookmarkBrowser::~QtBookmarkBrowser()
{
}

void QtBookmarkBrowser::setupBookmarkBrowser()
{
	setStyleSheet((
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("window/window.css"))) +
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("bookmark_view/bookmark_view.css")))
	).c_str());

	m_headerBackground = new QWidget(m_window);
	m_headerBackground->setObjectName("header_background");
	m_headerBackground->setGeometry(0, 0, 0, 0);
	m_headerBackground->lower();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	{
		QVBoxLayout* headerLayout = new QVBoxLayout();
		headerLayout->setSpacing(0);
		headerLayout->setContentsMargins(25, 35, 25, 35);
		headerLayout->setAlignment(Qt::AlignTop);
		layout->addLayout(headerLayout);

		headerLayout->addStrut(150);

		QLabel* title = new QLabel("Bookmarks");
		title->setObjectName("title");
		headerLayout->addWidget(title);

		headerLayout->addSpacing(40);

		QLabel* filterLabel = new QLabel("Show:");
		filterLabel->setObjectName("filter_label");
		headerLayout->addWidget(filterLabel);

		m_filterComboBox = new QComboBox();
		m_filterComboBox->addItem("All");
		m_filterComboBox->addItem("Nodes");
		m_filterComboBox->addItem("Edges");
		m_filterComboBox->setObjectName("filter_box");
		headerLayout->addWidget(m_filterComboBox);

		connect(m_filterComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(filterOrOrderChanged(const QString&)));

		headerLayout->addSpacing(40);

		QLabel* orderLabel = new QLabel("Sort by:");
		orderLabel->setObjectName("order_label");
		headerLayout->addWidget(orderLabel);

		m_orderNames.push_back("Name asc.");
		m_orderNames.push_back("Name des.");
		m_orderNames.push_back("Date asc.");
		m_orderNames.push_back("Date des.");

		m_orderComboBox = new QComboBox(this);
		// m_orderComboBox->setToolTip("Select Bookmark Order");
		m_orderComboBox->addItem(m_orderNames[0].c_str());
		m_orderComboBox->addItem(m_orderNames[1].c_str());
		m_orderComboBox->addItem(m_orderNames[2].c_str());
		m_orderComboBox->addItem(m_orderNames[3].c_str());
		m_orderComboBox->setObjectName("order_box");
		headerLayout->addWidget(m_orderComboBox);

		connect(m_orderComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(filterOrOrderChanged(const QString&)));
	}

	{
		QVBoxLayout* bodyLayout = new QVBoxLayout();
		bodyLayout->setSpacing(0);
		bodyLayout->setContentsMargins(0, 10, 0, 10);
		bodyLayout->setAlignment(Qt::AlignLeft);
		layout->addLayout(bodyLayout);

		m_bookmarkTree = new QTreeWidget();
		m_bookmarkTree->setObjectName("bookmark_tree");
		m_bookmarkTree->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_bookmarkTree->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
		m_bookmarkTree->header()->close();
		m_bookmarkTree->setIndentation(0);
		m_bookmarkTree->setHeaderLabel("Bookmarks");

		connect(m_bookmarkTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeItemClicked(QTreeWidgetItem*, int)));

		bodyLayout->addWidget(m_bookmarkTree);

		bodyLayout->addSpacing(15);

		QHBoxLayout* buttonLayout = createButtons();
		buttonLayout->setContentsMargins(0, 0, 23, 13);
		bodyLayout->addLayout(buttonLayout);
		setPreviousVisible(false);
		setCloseVisible(false);
		updateNextButton("Close");
	}
}

void QtBookmarkBrowser::setBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	std::map<QString, bool> wasCategoryExpanded;
	for (int i = 0; i < m_bookmarkTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_bookmarkTree->topLevelItem(i);
		wasCategoryExpanded.emplace(item->whatsThis(0), item->isExpanded());
	}

	m_bookmarkTree->clear();

	std::map<std::string, BookmarkCategory> categoryNamesOrdered;
	for (std::shared_ptr<Bookmark> bookmark: bookmarks)
	{
		categoryNamesOrdered.emplace(bookmark->getCategory().getName(), bookmark->getCategory());
	}

	for (auto p : categoryNamesOrdered)
	{
		findOrCreateTreeCategory(p.second);
	}

	for (std::shared_ptr<Bookmark> bookmark: bookmarks)
	{
		QtBookmark* qtBookmark = new QtBookmark();
		qtBookmark->setBookmark(bookmark);

		QTreeWidgetItem* categoryItem = findOrCreateTreeCategory(bookmark->getCategory());
		QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(categoryItem);
		m_bookmarkTree->setItemWidget(treeWidgetItem, 0, qtBookmark);
		categoryItem->addChild(treeWidgetItem);
		qtBookmark->setTreeWidgetItem(categoryItem);

		bool wasExpanded = true;
		auto it = wasCategoryExpanded.find(categoryItem->whatsThis(0));
		if (it != wasCategoryExpanded.end())
		{
			wasExpanded = it->second;
		}

		categoryItem->setExpanded(!wasExpanded);
		categoryItem->setExpanded(wasExpanded);
	}
}

void QtBookmarkBrowser::resizeEvent(QResizeEvent* event)
{
	QtWindow::resizeEvent(event);

	m_headerBackground->setGeometry(0, 0, 200, m_window->size().height());
}

void QtBookmarkBrowser::handleClose()
{
	close();
}

void QtBookmarkBrowser::handleNext()
{
	close();
}

void QtBookmarkBrowser::filterOrOrderChanged(const QString& text)
{
	MessageDisplayBookmarks(getSelectedFilter(), getSelectedOrder()).dispatch();
}

void  QtBookmarkBrowser::treeItemClicked(QTreeWidgetItem* item, int column)
{
	QtBookmarkCategory* category = dynamic_cast<QtBookmarkCategory*>(m_bookmarkTree->itemWidget(item, 0));
	if (category != NULL)
	{
		category->expandClicked();
		return;
	}

	QtBookmark* bookmark = dynamic_cast<QtBookmark*>(m_bookmarkTree->itemWidget(item, 0));
	if (bookmark != NULL)
	{
		bookmark->commentToggled();
		return;
	}
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

QTreeWidgetItem* QtBookmarkBrowser::findOrCreateTreeCategory(const BookmarkCategory& category)
{
	for (int i = 0; i < m_bookmarkTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_bookmarkTree->topLevelItem(i);

		if (item->whatsThis(0).toStdString() == category.getName())
		{
			return item;
		}
	}

	QtBookmarkCategory* categoryItem = new QtBookmarkCategory();
	if (category.getName().length() > 0)
	{
		categoryItem->setName(category.getName());
	}
	else
	{
		categoryItem->setName("No Category");
	}
	categoryItem->setId(category.getId());

	QTreeWidgetItem* newItem = new QTreeWidgetItem(m_bookmarkTree);
	newItem->setWhatsThis(0, category.getName().c_str());

	categoryItem->setTreeWidgetItem(newItem);

	m_bookmarkTree->setItemWidget(newItem, 0, categoryItem);
	m_bookmarkTree->addTopLevelItem(newItem);

	return newItem;
}
