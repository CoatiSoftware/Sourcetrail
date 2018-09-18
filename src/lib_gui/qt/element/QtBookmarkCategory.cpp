#include "QtBookmarkCategory.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>

#include "utilityQt.h"
#include "ResourcePaths.h"

QtBookmarkCategory::QtBookmarkCategory(ControllerProxy<BookmarkController>* controllerProxy)
	: m_controllerProxy(controllerProxy)
	, m_id(0)
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
	m_expandButton->setIcon(QPixmap(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/arrow_down.png").wstr())));
	m_expandButton->setIconSize(QSize(8, 8));
	layout->addWidget(m_expandButton);

	connect(m_expandButton, &QPushButton::clicked, this, &QtBookmarkCategory::expandClicked);

	m_name = new QLabel();
	m_name->setObjectName("category_name");
	layout->addWidget(m_name);

	layout->addStretch();

	m_deleteButton = new QPushButton();
	m_deleteButton->setObjectName("category_delete_button");
	m_deleteButton->setToolTip("Delete this Bookmark Category and the containing Bookmarks");
	m_deleteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_deleteButton->setIconSize(QSize(20, 20));
	m_deleteButton->setIcon(QPixmap(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/bookmark_delete_icon.png").wstr())));
	utility::setWidgetRetainsSpaceWhenHidden(m_deleteButton);
	m_deleteButton->hide();
	layout->addWidget(m_deleteButton);

	connect(m_deleteButton, &QPushButton::clicked, this, &QtBookmarkCategory::deleteClicked);
}

QtBookmarkCategory::~QtBookmarkCategory()
{
}

void QtBookmarkCategory::setName(const std::wstring& name)
{
	m_name->setText(QString::fromStdWString(name));
}

std::wstring QtBookmarkCategory::getName() const
{
	return m_name->text().toStdWString();
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
	if (m_treeItem != nullptr)
	{
		if (m_treeItem->isExpanded())
		{
			QPixmap pixmap(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/arrow_down.png").wstr()));
			m_expandButton->setIcon(QIcon(utility::colorizePixmap(pixmap, "black")));
		}
		else
		{
			QPixmap pixmap(QString::fromStdWString(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/arrow_right.png").wstr()));
			m_expandButton->setIcon(QIcon(utility::colorizePixmap(pixmap, "black")));
		}
	}
}

void QtBookmarkCategory::expandClicked()
{
	if (m_treeItem != nullptr)
	{
		m_treeItem->setExpanded(!m_treeItem->isExpanded());
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
		m_controllerProxy->executeAsTaskWithArgs(&BookmarkController::deleteBookmarkCategory, m_id);
	}
}
