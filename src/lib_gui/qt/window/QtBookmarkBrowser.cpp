#include "QtBookmarkBrowser.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "QtBookmark.h"
#include "QtBookmarkCategory.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtBookmarkBrowser::QtBookmarkBrowser(ControllerProxy<BookmarkController>* controllerProxy, QWidget* parent)
	: QtWindow(false, parent), m_controllerProxy(controllerProxy)
{
}

QtBookmarkBrowser::~QtBookmarkBrowser() {}

void QtBookmarkBrowser::setupBookmarkBrowser()
{
	setStyleSheet(
		(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")) +
		 utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/"
																		L"bookmark_view.css")))
			.c_str());

	m_headerBackground = new QWidget(m_window);
	m_headerBackground->setObjectName(QStringLiteral("header_background"));
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

		QLabel* title = new QLabel(QStringLiteral("Bookmarks"));
		title->setObjectName(QStringLiteral("title"));
		headerLayout->addWidget(title);

		headerLayout->addSpacing(40);

		QLabel* filterLabel = new QLabel(QStringLiteral("Show:"));
		filterLabel->setObjectName(QStringLiteral("filter_label"));
		headerLayout->addWidget(filterLabel);

		m_filterComboBox = new QComboBox();
		m_filterComboBox->addItem(QStringLiteral("All"));
		m_filterComboBox->addItem(QStringLiteral("Nodes"));
		m_filterComboBox->addItem(QStringLiteral("Edges"));
		m_filterComboBox->setObjectName(QStringLiteral("filter_box"));
		headerLayout->addWidget(m_filterComboBox);

		connect(
			m_filterComboBox,
			QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
			this,
			&QtBookmarkBrowser::filterOrOrderChanged);

		headerLayout->addSpacing(40);

		QLabel* orderLabel = new QLabel(QStringLiteral("Sort by:"));
		orderLabel->setObjectName(QStringLiteral("order_label"));
		headerLayout->addWidget(orderLabel);

		m_orderNames.push_back("Name asc.");
		m_orderNames.push_back("Name des.");
		m_orderNames.push_back("Date asc.");
		m_orderNames.push_back("Date des.");

		m_orderComboBox = new QComboBox(this);
		m_orderComboBox->addItem(m_orderNames[0].c_str());
		m_orderComboBox->addItem(m_orderNames[1].c_str());
		m_orderComboBox->addItem(m_orderNames[2].c_str());
		m_orderComboBox->addItem(m_orderNames[3].c_str());
		m_orderComboBox->setCurrentIndex(3);
		m_orderComboBox->setObjectName(QStringLiteral("order_box"));
		headerLayout->addWidget(m_orderComboBox);

		connect(
			m_orderComboBox,
			QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
			this,
			&QtBookmarkBrowser::filterOrOrderChanged);
	}

	{
		QVBoxLayout* bodyLayout = new QVBoxLayout();
		bodyLayout->setSpacing(0);
		bodyLayout->setContentsMargins(0, 10, 0, 10);
		bodyLayout->setAlignment(Qt::AlignLeft);
		layout->addLayout(bodyLayout);

		m_bookmarkTree = new QTreeWidget();
		m_bookmarkTree->setObjectName(QStringLiteral("bookmark_tree"));
		m_bookmarkTree->setAttribute(Qt::WA_MacShowFocusRect, 0);
		m_bookmarkTree->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
		m_bookmarkTree->header()->close();
		m_bookmarkTree->setIndentation(0);
		m_bookmarkTree->setHeaderLabel(QStringLiteral("Bookmarks"));

		connect(m_bookmarkTree, &QTreeWidget::itemClicked, this, &QtBookmarkBrowser::treeItemClicked);

		bodyLayout->addWidget(m_bookmarkTree);

		bodyLayout->addSpacing(15);

		QHBoxLayout* buttonLayout = createButtons();
		buttonLayout->setContentsMargins(0, 0, 23, 13);
		bodyLayout->addLayout(buttonLayout);
		setPreviousVisible(false);
		setCloseVisible(false);
		updateNextButton(QStringLiteral("Close"));
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

	std::map<std::wstring, BookmarkCategory> categoryNamesOrdered;
	for (const std::shared_ptr<Bookmark>& bookmark: bookmarks)
	{
		categoryNamesOrdered.emplace(bookmark->getCategory().getName(), bookmark->getCategory());
	}

	for (const auto& p: categoryNamesOrdered)
	{
		findOrCreateTreeCategory(p.second);
	}

	for (const std::shared_ptr<Bookmark>& bookmark: bookmarks)
	{
		QtBookmark* qtBookmark = new QtBookmark(m_controllerProxy);
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
	Bookmark::BookmarkFilter filter = getSelectedFilter();
	Bookmark::BookmarkOrder order = getSelectedOrder();

	m_controllerProxy->executeAsTaskWithArgs(&BookmarkController::displayBookmarksFor, filter, order);
}

void QtBookmarkBrowser::treeItemClicked(QTreeWidgetItem* item, int column)
{
	QtBookmarkCategory* category = dynamic_cast<QtBookmarkCategory*>(
		m_bookmarkTree->itemWidget(item, 0));
	if (category != nullptr)
	{
		category->expandClicked();
		return;
	}

	QtBookmark* bookmark = dynamic_cast<QtBookmark*>(m_bookmarkTree->itemWidget(item, 0));
	if (bookmark != nullptr)
	{
		bookmark->commentToggled();
		return;
	}
}

Bookmark::BookmarkFilter QtBookmarkBrowser::getSelectedFilter()
{
	std::string text = m_filterComboBox->currentText().toStdString();

	if (text == "Nodes")
	{
		return Bookmark::FILTER_NODES;
	}
	else if (text == "Edges")
	{
		return Bookmark::FILTER_EDGES;
	}

	return Bookmark::FILTER_ALL;
}

Bookmark::BookmarkOrder QtBookmarkBrowser::getSelectedOrder()
{
	std::string orderString = m_orderComboBox->currentText().toStdString();

	if (orderString == m_orderNames[0])
	{
		return Bookmark::ORDER_NAME_ASCENDING;
	}
	else if (orderString == m_orderNames[1])
	{
		return Bookmark::ORDER_NAME_DESCENDING;
	}
	else if (orderString == m_orderNames[2])
	{
		return Bookmark::ORDER_DATE_ASCENDING;
	}
	else if (orderString == m_orderNames[3])
	{
		return Bookmark::ORDER_DATE_DESCENDING;
	}
	else
	{
		return Bookmark::ORDER_NONE;
	}
}

QTreeWidgetItem* QtBookmarkBrowser::findOrCreateTreeCategory(const BookmarkCategory& category)
{
	for (int i = 0; i < m_bookmarkTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = m_bookmarkTree->topLevelItem(i);

		if (item->whatsThis(0).toStdWString() == category.getName())
		{
			return item;
		}
	}

	QtBookmarkCategory* categoryItem = new QtBookmarkCategory(m_controllerProxy);
	if (category.getName().length() > 0)
	{
		categoryItem->setName(category.getName());
	}
	else
	{
		categoryItem->setName(L"No Category");
	}
	categoryItem->setId(category.getId());

	QTreeWidgetItem* newItem = new QTreeWidgetItem(m_bookmarkTree);
	newItem->setWhatsThis(0, QString::fromStdWString(category.getName()));

	categoryItem->setTreeWidgetItem(newItem);

	m_bookmarkTree->setItemWidget(newItem, 0, categoryItem);
	m_bookmarkTree->addTopLevelItem(newItem);

	return newItem;
}
