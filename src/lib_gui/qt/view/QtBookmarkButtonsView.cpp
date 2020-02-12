#include "QtBookmarkButtonsView.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include "MessageBookmarkBrowse.h"
#include "MessageBookmarkCreate.h"
#include "MessageBookmarkDelete.h"
#include "MessageBookmarkEdit.h"
#include "QtSearchBarButton.h"
#include "QtViewWidgetWrapper.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtBookmarkButtonsView::QtBookmarkButtonsView(ViewLayout* viewLayout)
	: BookmarkButtonsView(viewLayout), m_createButtonState(MessageBookmarkButtonState::CANNOT_CREATE)
{
	m_widget = new QFrame();
	m_widget->setObjectName(QStringLiteral("bookmark_bar"));

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	m_widget->setLayout(layout);

	m_createBookmarkButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/edit_bookmark_icon.png"));
	m_createBookmarkButton->setObjectName(QStringLiteral("bookmark_button"));
	m_createBookmarkButton->setToolTip(QStringLiteral("create a bookmark for the active symbol"));
	m_createBookmarkButton->setEnabled(false);
	layout->addWidget(m_createBookmarkButton);

	connect(
		m_createBookmarkButton,
		&QPushButton::clicked,
		this,
		&QtBookmarkButtonsView::createBookmarkClicked);

	m_showBookmarksButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"bookmark_view/images/bookmark_list_icon.png"));
	m_showBookmarksButton->setObjectName(QStringLiteral("show_bookmark_button"));
	m_showBookmarksButton->setToolTip(QStringLiteral("Show bookmarks"));
	layout->addWidget(m_showBookmarksButton);

	connect(
		m_showBookmarksButton,
		&QPushButton::clicked,
		this,
		&QtBookmarkButtonsView::showBookmarksClicked);
}

void QtBookmarkButtonsView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtBookmarkButtonsView::refreshView()
{
	m_onQtThread([=]() {
		m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(
														   L"bookmark_view/bookmark_view.css"))
									.c_str());
	});
}

void QtBookmarkButtonsView::setCreateButtonState(const MessageBookmarkButtonState::ButtonState& state)
{
	m_onQtThread([=]() {
		m_createButtonState = state;

		m_createBookmarkButton->setIconPath(ResourcePaths::getGuiPath().concatenate(
			L"bookmark_view/images/edit_bookmark_icon.png"));

		if (state == MessageBookmarkButtonState::CAN_CREATE)
		{
			m_createBookmarkButton->setEnabled(true);
		}
		else if (state == MessageBookmarkButtonState::CANNOT_CREATE)
		{
			m_createBookmarkButton->setEnabled(false);
		}
		else if (state == MessageBookmarkButtonState::ALREADY_CREATED)
		{
			m_createBookmarkButton->setEnabled(true);

			m_createBookmarkButton->setIconPath(ResourcePaths::getGuiPath().concatenate(
				L"bookmark_view/images/bookmark_active.png"));
		}
		else
		{
			m_createBookmarkButton->setEnabled(false);
		}
	});
}

void QtBookmarkButtonsView::createBookmarkClicked()
{
	if (m_createButtonState == MessageBookmarkButtonState::CAN_CREATE)
	{
		MessageBookmarkCreate().dispatch();
	}
	else if (m_createButtonState == MessageBookmarkButtonState::ALREADY_CREATED)
	{
		QMessageBox msgBox;
		msgBox.setText(QStringLiteral("Edit Bookmark"));
		msgBox.setInformativeText(
			QStringLiteral("Do you want to edit or delete the bookmark for this symbol?"));
		msgBox.addButton(QStringLiteral("Edit"), QMessageBox::ButtonRole::YesRole);
		msgBox.addButton(QStringLiteral("Delete"), QMessageBox::ButtonRole::NoRole);
		QPushButton* cancelButton = msgBox.addButton(
			QStringLiteral("Cancel"), QMessageBox::ButtonRole::RejectRole);
		msgBox.setDefaultButton(cancelButton);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0)	 // QMessageBox::Yes
		{
			MessageBookmarkEdit().dispatch();
		}
		else if (ret == 1)
		{
			MessageBookmarkDelete().dispatch();
		}
	}
}

void QtBookmarkButtonsView::showBookmarksClicked()
{
	MessageBookmarkBrowse().dispatch();
}
