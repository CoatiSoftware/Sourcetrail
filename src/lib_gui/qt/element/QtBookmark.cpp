#include "QtBookmark.h"

#include <QMessageBox>
#include <QPixmap>

#include "qt/window/QtBookmarkCreator.h"

#include "data/bookmark/EdgeBookmark.h"

#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageDisplayBookmarkEditor.h"

#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"

QtBookmark::QtBookmark()
	: m_treeWidgetItem(NULL)
{
	setObjectName("bookmark");

	m_layout = new QVBoxLayout();
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setAlignment(Qt::AlignTop);
	setLayout(m_layout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->setSpacing(0);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setAlignment(Qt::AlignTop);

	m_toggleCommentButton = new QPushButton();
	m_toggleCommentButton->setObjectName("comment_button");
	m_toggleCommentButton->setToolTip("Show Comment");
	m_toggleCommentButton->setText("");
	m_toggleCommentButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	buttonsLayout->addWidget(m_toggleCommentButton);

	m_dateLabel = new QLabel();
	m_dateLabel->setObjectName("date_label");
	m_dateLabel->setText("n/a");
	buttonsLayout->addWidget(m_dateLabel);

	m_activateButton = new QPushButton();
	m_activateButton->setObjectName("activate_button");
	m_activateButton->setToolTip("Activate bookmark");
	m_activateButton->setText("Bookmark");
	m_activateButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	buttonsLayout->addWidget(m_activateButton);

	m_editButton = new QPushButton();
	m_editButton->setObjectName("edit_button");
	m_editButton->setToolTip("Edit bookmark");
	m_editButton->setText("Edit");
	m_editButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	buttonsLayout->addWidget(m_editButton);

	m_deleteButton = new QPushButton();
	m_deleteButton->setObjectName("delete_button");
	m_deleteButton->setToolTip("Delete bookmark");
	m_deleteButton->setText("Delete");
	m_deleteButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	buttonsLayout->addWidget(m_deleteButton);

	m_layout->addLayout(buttonsLayout);

	m_comment = new QLabel();
	m_comment->setText("no comment");
	m_comment->hide();
	m_comment->setWordWrap(true);
	m_layout->addWidget(m_comment);


	connect(m_activateButton, SIGNAL(clicked()), this, SLOT(activateClicked()));
	connect(m_editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
	connect(m_toggleCommentButton, SIGNAL(clicked()), this, SLOT(commentToggled()));

	refreshStyle();
}

QtBookmark::~QtBookmark()
{
}

void QtBookmark::setName(const std::string& name)
{
	m_activateButton->setText(name.c_str());
}

std::string QtBookmark::getName() const
{
	return m_bookmark->getDisplayName();
}

void QtBookmark::setBookmark(const std::shared_ptr<Bookmark> bookmark)
{
	m_bookmark = bookmark;

	m_activateButton->setText(bookmark->getDisplayName().c_str());

	if (m_bookmark->isValid() == false)
	{
		m_activateButton->setEnabled(false);
		m_editButton->setEnabled(false);
	}

	if (m_bookmark->getComment().length() > 0)
	{
		m_comment->setText(m_bookmark->getComment().c_str());
		m_activateButton->setToolTip(bookmark->getComment().c_str());
	}
	m_dateLabel->setText(getDateString().c_str());

	getDateString();
}

Id QtBookmark::getBookmarkId() const
{
	return m_bookmark->getId();
}

QTreeWidgetItem* QtBookmark::getTreeWidgetItem() const
{
	return m_treeWidgetItem;
}

void QtBookmark::setTreeWidgetItem(QTreeWidgetItem* treeWidgetItem)
{
	m_treeWidgetItem = treeWidgetItem;
}

void QtBookmark::refreshStyle()
{
	if (m_editButton != NULL)
	{
		m_editButton->setText("");
		m_editButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_edit_icon.png").c_str()));
	}

	if (m_deleteButton != NULL)
	{
		m_deleteButton->setText("");
		m_deleteButton->setIcon(QPixmap((ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_delete_icon.png").c_str()));
	}

	if (m_toggleCommentButton != NULL)
	{
		m_toggleCommentButton->setText("");
		m_toggleCommentButton->setIcon(utility::createButtonIcon(
			ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_right.png",
			"bookmark/button"
		));
		m_toggleCommentButton->setIconSize(QSize(10, 10));
	}
}

void QtBookmark::activateClicked()
{
	MessageActivateBookmark(m_bookmark).dispatch();
}

void QtBookmark::editClicked()
{
	MessageDisplayBookmarkEditor(m_bookmark).dispatch();
}

void QtBookmark::deleteClicked()
{
	QMessageBox msgBox;
	msgBox.setText("Delete Bookmark");
	msgBox.setInformativeText("Do you really want to delete this bookmark?");
	msgBox.addButton("Delete", QMessageBox::ButtonRole::YesRole);
	msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0) // QMessageBox::Yes)
	{
		MessageDeleteBookmark(m_bookmark->getId(), (dynamic_cast<EdgeBookmark*>(m_bookmark.get()) != NULL)).dispatch();
	}
}

void QtBookmark::commentToggled()
{
	// std::string text = m_toggleCommentButton->text().toStdString();

	if (m_comment->isVisible() == false)
	{
		m_toggleCommentButton->setText("");
		m_toggleCommentButton->setText("");
		m_toggleCommentButton->setIcon(utility::createButtonIcon(
			ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_down.png",
			"bookmark/button"
		));
		m_toggleCommentButton->setIconSize(QSize(10, 10));
		m_comment->show();
	}
	else
	{
		m_toggleCommentButton->setText("");
		m_toggleCommentButton->setText("");
		m_toggleCommentButton->setIcon(utility::createButtonIcon(
			ResourcePaths::getGuiPath() + "bookmark_view/images/arrow_right.png",
			"bookmark/button"
		));
		m_toggleCommentButton->setIconSize(QSize(10, 10));
		m_comment->hide();
	}

	// forces the parent tree view to rescale
	if (m_treeWidgetItem)
	{
		m_treeWidgetItem->setExpanded(false);
		m_treeWidgetItem->setExpanded(true);
	}
}

void QtBookmark::handleMessage(MessageEditBookmark* message)
{
	if (m_bookmark->getId() == message->bookmarkId)
	{
		m_bookmark->setDisplayName(message->displayName);
		m_bookmark->setComment(message->comment);

		m_activateButton->setText(message->displayName.c_str());
	}
}

std::string QtBookmark::getDateString() const
{
	std::string result = "n/a";

	TimePoint creationDate = m_bookmark->getTimeStamp();

	float delta = TimePoint::now() - creationDate;

	if (delta < 3600.0f) // less than an hour ago
	{
		result = std::to_string(int(delta / 60.0f)) + " minutes ago";
	}
	else if (delta < (3600.0f * 6.0f)) // less than 6 hours ago
	{
		result = std::to_string(int(delta / 3600.0f)) + " hours ago";
	}
	else if (creationDate.isSameDay(TimePoint::now())) // today
	{
		result = "today";
	}
	else if(creationDate.deltaDays(TimePoint::now()) == 1) // yesterday
	{
		result = "yesterday";
	}
	else // whenever
	{
		result = creationDate.getDDMMYYYYString();
	}

	return result;
}
