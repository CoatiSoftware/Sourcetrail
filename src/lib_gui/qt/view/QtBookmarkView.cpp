#include "QtBookmarkView.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include "QtSearchBarButton.h"
#include "utilityQt.h"
#include "QtMainView.h"
#include "QtViewWidgetWrapper.h"
#include "QtBookmarkBrowser.h"
#include "QtBookmarkCreator.h"
#include "QtMainWindow.h"
#include "ResourcePaths.h"

QtBookmarkView::QtBookmarkView(ViewLayout* viewLayout)
	: BookmarkView(viewLayout)
	, m_controllerProxy(this)
	, m_bookmarkBrowser(nullptr)
	, m_createButtonState(BookmarkView::CreateButtonState::CANNOT_CREATE)
{
	m_widget = new QFrame();
}

QtBookmarkView::~QtBookmarkView()
{
}

void QtBookmarkView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtBookmarkView::initView()
{
	m_widget->setObjectName("bookmark_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	m_widget->setLayout(layout);

	m_createBookmarkButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/edit_bookmark_icon.png"));
	m_createBookmarkButton->setObjectName("bookmark_button");
	m_createBookmarkButton->setToolTip("create a bookmark for the active symbol");
	m_createBookmarkButton->setEnabled(false);
	layout->addWidget(m_createBookmarkButton);

	connect(m_createBookmarkButton, &QPushButton::clicked, this, &QtBookmarkView::createBookmarkClicked);

	m_showBookmarksButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/bookmark_list_icon.png"));
	m_showBookmarksButton->setObjectName("show_bookmark_button");
	m_showBookmarksButton->setToolTip("Show bookmarks");
	m_showBookmarksButton->setEnabled(false);
	layout->addWidget(m_showBookmarksButton);

	connect(m_showBookmarksButton, &QPushButton::clicked, this, &QtBookmarkView::showBookmarksClicked);
}

void QtBookmarkView::refreshView()
{
	m_onQtThread(
		[=]()
		{
			m_widget->setStyleSheet(utility::getStyleSheet(
				ResourcePaths::getGuiPath().concatenate(L"bookmark_view/bookmark_view.css")
			).c_str());
		}
	);
}

void QtBookmarkView::setCreateButtonState(const CreateButtonState& state)
{
	m_onQtThread(
		[=]()
		{
			m_createButtonState = state;

			m_createBookmarkButton->setIconPath(
				ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/edit_bookmark_icon.png"));

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

				m_createBookmarkButton->setIconPath(
					ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/bookmark_active.png"));
			}
			else
			{
				m_createBookmarkButton->setEnabled(false);
			}
		}
	);
}

void QtBookmarkView::displayBookmarkCreator(
	const std::vector<std::wstring>& names, const std::vector<BookmarkCategory>& categories, Id nodeId
){
	m_onQtThread(
		[=]()
		{
			QtBookmarkCreator* bookmarkCreator = new QtBookmarkCreator(
				&m_controllerProxy,
				dynamic_cast<QtMainView*>(dynamic_cast<View*>(getViewLayout())->getViewLayout())->getMainWindow()
			);
			bookmarkCreator->setupBookmarkCreator();

			std::wstring displayName = L"";

			for (unsigned int i = 0; i < names.size(); i++)
			{
				displayName += names[i];

				if (i < names.size() - 1)
				{
					displayName += L"; ";
				}
			}

			bookmarkCreator->setDisplayName(displayName);
			bookmarkCreator->setBookmarkCategories(categories);
			bookmarkCreator->setNodeId(nodeId);

			bookmarkCreator->show();
			bookmarkCreator->raise();
		}
	);
}

void QtBookmarkView::displayBookmarkEditor(
	std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories
){
	m_onQtThread(
		[=]()
		{
			QtBookmarkCreator* bookmarkCreator = new QtBookmarkCreator(
				&m_controllerProxy,
				dynamic_cast<QtMainView*>(dynamic_cast<View*>(getViewLayout())->getViewLayout())->getMainWindow(),
				bookmark->getId()
			);

			bookmarkCreator->setupBookmarkCreator();
			bookmarkCreator->setDisplayName(bookmark->getName());
			bookmarkCreator->setComment(bookmark->getComment());
			bookmarkCreator->setBookmarkCategories(categories);
			bookmarkCreator->setCurrentBookmarkCategory(bookmark->getCategory());

			bookmarkCreator->show();
			bookmarkCreator->raise();
		}
	);
}

void QtBookmarkView::displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_onQtThread(
		[=]()
		{
			if (m_bookmarkBrowser == nullptr)
			{
				m_bookmarkBrowser = new QtBookmarkBrowser(
					&m_controllerProxy,
					dynamic_cast<QtMainView*>(dynamic_cast<View*>(getViewLayout())->getViewLayout())->getMainWindow()
				);
				m_bookmarkBrowser->setupBookmarkBrowser();
			}

			m_bookmarkBrowser->setBookmarks(bookmarks);
			m_bookmarkBrowser->show();
			m_bookmarkBrowser->raise();
		}
	);
}

void QtBookmarkView::enableDisplayBookmarks(bool enable)
{
	m_onQtThread(
		[=]()
		{
			m_showBookmarksButton->setEnabled(enable);
		}
	);
}

bool QtBookmarkView::bookmarkBrowserIsVisible() const
{
	if (m_bookmarkBrowser != nullptr)
	{
		return m_bookmarkBrowser->isVisible();
	}
	else
	{
		return false;
	}
}

void QtBookmarkView::createBookmarkClicked()
{
	if (m_createButtonState == BookmarkView::CreateButtonState::CAN_CREATE)
	{
		m_controllerProxy.executeAsTaskWithArgs(&BookmarkController::showBookmarkCreator, 0);
	}
	else if (m_createButtonState == BookmarkView::CreateButtonState::ALREADY_CREATED)
	{
		QMessageBox msgBox;
		msgBox.setText("Edit Bookmark");
		msgBox.setInformativeText("Do you want to edit or delete the bookmark for this symbol?");
		msgBox.addButton("Edit", QMessageBox::ButtonRole::YesRole);
		msgBox.addButton("Delete", QMessageBox::ButtonRole::NoRole);
		QPushButton* cancelButton = msgBox.addButton("Cancel", QMessageBox::ButtonRole::RejectRole);
		msgBox.setDefaultButton(cancelButton);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0) // QMessageBox::Yes
		{
			m_controllerProxy.executeAsTaskWithArgs(&BookmarkController::showBookmarkCreator, 0);
		}
		else if (ret == 1)
		{
			m_controllerProxy.executeAsTask(&BookmarkController::deleteBookmarkForActiveTokens);
		}
	}
}

void QtBookmarkView::showBookmarksClicked()
{
	m_controllerProxy.executeAsTask(&BookmarkController::displayBookmarks);
}
