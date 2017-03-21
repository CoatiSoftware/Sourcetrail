#include "QtBookmarkCategory.h"

#include <QHBoxLayout>
#include <QMessageBox>

#include "utility/messaging/type/MessageDeleteBookmarkCategoryWithBookmarks.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"

QtBookmarkCategory::QtBookmarkCategory()
	: m_id(0)
{
	setObjectName("bookmark_category");

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_expandButton = new QPushButton();
	m_expandButton->setObjectName("category_expand_button");
	m_expandButton->setToolTip("Show/Hide bookmarks in this category");
	m_expandButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_expandButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_down.png").c_str()));
	m_expandButton->setIconSize(QSize(10, 10));
	layout->addWidget(m_expandButton);

	connect(m_expandButton, SIGNAL(clicked()), this, SLOT(expandClicked()));

	m_name = new QLabel();
	m_name->setObjectName("category_name");
	layout->addWidget(m_name);

	layout->addStretch();

	m_deleteButton = new QPushButton();
	m_deleteButton->setObjectName("category_delete_button");
	m_deleteButton->setToolTip("Delete this Bookmark Category and the containing Bookmarks");
	m_deleteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_deleteButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_delete_icon.png").c_str()));
	utility::setWidgetRetainsSpaceWhenHidden(m_deleteButton);
	m_deleteButton->hide();
	layout->addWidget(m_deleteButton);

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
			QPixmap pixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_down.png").c_str());
			m_expandButton->setIcon(QIcon(utility::colorizePixmap(pixmap, "black")));
		}
		else
		{
			QPixmap pixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_right.png").c_str());
			m_expandButton->setIcon(QIcon(utility::colorizePixmap(pixmap, "black")));
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

void QtBookmarkCategory::enterEvent(QEvent *event)
{
	m_deleteButton->show();
}

void QtBookmarkCategory::leaveEvent(QEvent *event)
{
	m_deleteButton->hide();
}

void QtBookmarkCategory::deleteClicked()
{
	QMessageBox msgBox;
	msgBox.setText("Delete Category");
	msgBox.setInformativeText("Do you really want to delete this category AND all containing bookmarks?");
	msgBox.addButton("Delete", QMessageBox::ButtonRole::YesRole);
	msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0) // QMessageBox::Yes
	{
		MessageDeleteBookmarkCategoryWithBookmarks(m_id).dispatch();
	}
}
