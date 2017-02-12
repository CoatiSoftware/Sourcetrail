#include "QtBookmarkBar.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include "data/bookmark/EdgeBookmark.h"

#include "utility/messaging/type/MessageDeleteBookmarkForActiveTokens.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageDisplayBookmarkCreator.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

#include "qt/window/QtBookmarkCreator.h"

#include "qt/window/QtMainWindow.h"

#include "qt/element/QtBookmark.h"

QtBookmarkBar::QtBookmarkBar()
	: m_displayBookmarksFunctor(std::bind(&QtBookmarkBar::doDisplayBookmarks, this, std::placeholders::_1))
	, m_displayBookmarkCreatorFunctor(std::bind(&QtBookmarkBar::doDisplayBookmarkCreator, this, std::placeholders::_1, std::placeholders::_2))
	, m_displayBookmarkEditorFunctor(std::bind(&QtBookmarkBar::doDisplayBookmarkEditor, this, std::placeholders::_1, std::placeholders::_2))
	, m_setCreateButtonStateFunctor(std::bind(&QtBookmarkBar::doSetCreateButtonState, this, std::placeholders::_1))
	, m_enableDisplayButtonFunctor(std::bind(&QtBookmarkBar::doEnableDisplayButton, this, std::placeholders::_1))
	, m_bookmarkBrowser(NULL)
	, m_createButtonState(BookmarkView::CreateButtonState::CANNOT_CREATE)
{
	setObjectName("bookmark_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_createBookmarkButton = new QPushButton(this);
	m_createBookmarkButton->setObjectName("bookmark_button");
	m_createBookmarkButton->setToolTip("create a bookmark for the active symbol");
	m_createBookmarkButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_createBookmarkButton);

	m_createBookmarkButton->setEnabled(false);

	connect(m_createBookmarkButton, SIGNAL(clicked()), this, SLOT(createBookmarkClicked()));

	m_showBookmarksButton = new QPushButton(this);
	m_showBookmarksButton->setObjectName("show_bookmark_button");
	m_showBookmarksButton->setToolTip("Show bookmarks");
	m_showBookmarksButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
	layout->addWidget(m_showBookmarksButton);

	m_showBookmarksButton->setEnabled(false);

	connect(m_showBookmarksButton, SIGNAL(clicked()), this, SLOT(showBookmarksClicked()));

	refreshStyle();
}

QtBookmarkBar::~QtBookmarkBar()
{

}

void QtBookmarkBar::refreshStyle()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "bookmark_view/bookmark_view.css").c_str());

	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 16, 30);

	m_createBookmarkButton->setFixedHeight(height);
	m_showBookmarksButton->setFixedHeight(height);

	std::string createImage = ResourcePaths::getGuiPath() + "bookmark_view/images/edit_bookmark_icon.png";
	std::string listImage = ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_list_icon.png";

	m_createBookmarkButton->setIcon(utility::colorizePixmap(
		QPixmap(createImage.c_str()),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));

	m_showBookmarksButton->setIcon(utility::colorizePixmap(
		QPixmap(listImage.c_str()),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));
}

void QtBookmarkBar::displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_displayBookmarksFunctor(bookmarks);
}

void QtBookmarkBar::displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories)
{
	m_displayBookmarkCreatorFunctor(names, categories);
}

void QtBookmarkBar::displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories)
{
	m_displayBookmarkEditorFunctor(bookmark, categories);
}

void QtBookmarkBar::setCreateButtonState(const BookmarkView::CreateButtonState& state)
{
	m_setCreateButtonStateFunctor(state);
}

void QtBookmarkBar::enableDisplayButton(bool enable)
{
	m_enableDisplayButtonFunctor(enable);
}

bool QtBookmarkBar::bookmarkBrowserIsVisible() const
{
	if (m_bookmarkBrowser != NULL)
	{
		return m_bookmarkBrowser->isVisible();
	}
	else
	{
		return false;
	}
}

void QtBookmarkBar::createBookmarkClicked()
{
	if (m_createButtonState == BookmarkView::CreateButtonState::CAN_CREATE)
	{
		MessageDisplayBookmarkCreator().dispatch();
	}
	else if (m_createButtonState == BookmarkView::CreateButtonState::ALREADY_CREATED)
	{
		QMessageBox msgBox;
		msgBox.setText("Edit Bookmark");
		msgBox.setInformativeText("Do you want to update or delete the Bookmark for the active Token?");
		msgBox.addButton("Edit", QMessageBox::ButtonRole::YesRole);
		msgBox.addButton("Delete", QMessageBox::ButtonRole::DestructiveRole);
		msgBox.addButton("Cancel", QMessageBox::ButtonRole::NoRole);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0) // QMessageBox::Yes
		{
			MessageDisplayBookmarkCreator().dispatch();
		}
		else if (ret == 1)
		{
			MessageDeleteBookmarkForActiveTokens().dispatch();
		}
	}
}

void QtBookmarkBar::showBookmarksClicked()
{
	MessageDisplayBookmarks(MessageDisplayBookmarks::BookmarkFilter::ALL, MessageDisplayBookmarks::BookmarkOrder::NONE).dispatch();
}

void QtBookmarkBar::handleMessage(MessageEnteredLicense* message)
{
	m_onQtThread(
		[=]()
	{
		// actually only want to enable this when a project is loaded
		// maybe factor in licence later...
		// m_createBookmarkButton->setEnabled(true);
		// m_showBookmarksButton->setEnabled(true);
	}
	);
}

void QtBookmarkBar::doDisplayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories)
{
	QtBookmarkCreator* bookmarkCreator = new QtBookmarkCreator();
	bookmarkCreator->setupBookmarkCreator();

	std::string displayName = "";

	for (unsigned int i = 0; i < names.size(); i++)
	{
		displayName += names[i];

		if (i < names.size() - 1)
		{
			displayName += "; ";
		}
	}

	bookmarkCreator->setDisplayName(displayName);
	bookmarkCreator->setBookmarkCategories(categories);
	bookmarkCreator->show();
}

void QtBookmarkBar::doDisplayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories)
{
	QtBookmarkCreator* bookmarkCreator = new QtBookmarkCreator(NULL, true, bookmark->getId());
	bookmarkCreator->setupBookmarkCreator();
	bookmarkCreator->setDisplayName(bookmark->getDisplayName());
	bookmarkCreator->setComment(bookmark->getComment());
	bookmarkCreator->setBookmarkCategories(categories);
	bookmarkCreator->setCurrentBookmarkCategory(bookmark->getCategory());
	bookmarkCreator->setIsEdge((dynamic_cast<EdgeBookmark*>(bookmark.get()) != NULL));

	bookmarkCreator->show();
}

void QtBookmarkBar::doDisplayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	if (m_bookmarkBrowser == NULL)
	{
		m_bookmarkBrowser = new QtBookmarkBrowser();
		m_bookmarkBrowser->setupBookmarkBrowser();
	}

	m_bookmarkBrowser->setBookmarks(bookmarks);
	m_bookmarkBrowser->show();
}

void QtBookmarkBar::doSetCreateButtonState(const BookmarkView::CreateButtonState& state)
{
	m_createButtonState = state;

	std::string createImage = ResourcePaths::getGuiPath() + "bookmark_view/images/edit_bookmark_icon.png";

	m_createBookmarkButton->setIcon(utility::colorizePixmap(
		QPixmap(createImage.c_str()),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));

	if (state == BookmarkView::CreateButtonState::CAN_CREATE)
	{
		m_createBookmarkButton->setEnabled(true);
	}
	else if (state == BookmarkView::CreateButtonState::CANNOT_CREATE)
	{
		m_createBookmarkButton->setEnabled(false);
	}
	else if (state == BookmarkView::CreateButtonState::ALREADY_CREATED)
	{
		m_createBookmarkButton->setEnabled(true);

		std::string createImage = ResourcePaths::getGuiPath() + "bookmark_view/images/bookmark_active.png";

		m_createBookmarkButton->setIcon(utility::colorizePixmap(
			QPixmap(createImage.c_str()),
			ColorScheme::getInstance()->getColor("search/button/icon").c_str()
		));
	}
	else
	{
		m_createBookmarkButton->setEnabled(false);
	}
}

void QtBookmarkBar::doEnableDisplayButton(bool enable)
{
	m_showBookmarksButton->setEnabled(enable);
}