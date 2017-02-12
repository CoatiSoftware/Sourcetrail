#include "QtBookmarkCategory.h"

#include <QMessageBox>

#include "utility/messaging/type/MessageDeleteBookmarkCategoryWithBookmarks.h"

#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"

QtBookmarkCategory::QtBookmarkCategory()
	: m_name(NULL)
	, m_layout(NULL)
	, m_deleteButton(NULL)
	, m_id(-1)
	, m_treeItem(NULL)
{
	setObjectName("bookmark_category");

	m_layout = new QHBoxLayout();
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setAlignment(Qt::AlignTop);
	setLayout(m_layout);

	m_expandButton = new QPushButton();
	m_expandButton->setObjectName("category_expand_button");
	m_expandButton->setToolTip("Show/Hide bookmarks in this category");
	m_expandButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_down.png",
		"bookmark/button"
	));
	m_expandButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_layout->addWidget(m_expandButton);

	connect(m_expandButton, SIGNAL(clicked()), this, SLOT(expandClicked()));

	m_name = new QLabel();
	m_name->setObjectName("category_name");
	m_name->setText("");
	m_layout->addWidget(m_name);

	m_deleteButton = new QPushButton();
	m_deleteButton->setObjectName("category_delete_button");
	m_deleteButton->setToolTip("Delete this Bookmark Category and the containing Bookmarks");
	// m_deleteButton->setText("Delete");
	m_deleteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_deleteButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_delete_icon.png").c_str()));
	m_layout->addWidget(m_deleteButton);

	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
}

QtBookmarkCategory::~QtBookmarkCategory()
{
}

void QtBookmarkCategory::setName(const std::string& name)
{
	if (m_name != NULL)
	{
		m_name->setText(name.c_str());
	}
}

std::string QtBookmarkCategory::getName() const
{
	std::string result = "";

	if (m_name != NULL)
	{
		result = m_name->text().toStdString();
	}

	return result;
}

void QtBookmarkCategory::setId(const Id id)
{
	m_id = id;
}

Id QtBookmarkCategory::getId() const
{
	return m_id;
}

void QtBookmarkCategory::setTreeWidgetItem(QTreeWidgetItem* treeItem)
{
	m_treeItem = treeItem;
}

void QtBookmarkCategory::updateArrow()
{
	if (m_treeItem != NULL)
	{
		if (m_treeItem->isExpanded())
		{
			m_expandButton->setIcon(utility::createButtonIcon(
				ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_down.png",
				"bookmark/button"
			));
		}
		else
		{
			m_expandButton->setIcon(utility::createButtonIcon(
				ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_right.png",
				"bookmark/button"
			));
		}
	}
}

void QtBookmarkCategory::expandClicked()
{
	if (m_treeItem != NULL)
	{
		if (m_treeItem->isExpanded())
		{
			m_treeItem->setExpanded(false);
		}
		else
		{
			m_treeItem->setExpanded(true);
		}

		updateArrow();
	}
}

void QtBookmarkCategory::deleteClicked()
{
	QMessageBox msgBox;
	msgBox.setText("Delete Category");
	msgBox.setInformativeText("Do you really want to delete this category AND all containing bookmarks?");
	QAbstractButton* yesButton = msgBox.addButton("Delete", QMessageBox::ButtonRole::YesRole);
	QAbstractButton* noButton = msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0)
	{
		MessageDeleteBookmarkCategoryWithBookmarks(m_id).dispatch();
	}
}