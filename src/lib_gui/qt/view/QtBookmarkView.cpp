#include "qt/view/QtBookmarkView.h"

#include "qt/window/QtBookmarkBrowser.h"
#include "qt/utility/utilityQt.h"

#include "utility/ResourcePaths.h"

#include "component/controller/RefreshController.h"
#include "qt/view/QtViewWidgetWrapper.h"

#include "settings/ColorScheme.h"

QtBookmarkView::QtBookmarkView(ViewLayout* viewLayout)
	: BookmarkView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtBookmarkView::doRefreshView, this))
{
	m_widget = new QtBookmarkBar();
	setStyleSheet();
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

}

void QtBookmarkView::refreshView()
{
	m_refreshViewFunctor();
}

void QtBookmarkView::setCreateButtonState(const CreateButtonState& state)
{
	m_widget->setCreateButtonState(state);
}

void QtBookmarkView::enableDisplayBookmarks(bool enable)
{
	m_widget->enableDisplayButton(enable);
}

bool QtBookmarkView::bookmarkBrowserIsVisible() const
{
	return m_widget->bookmarkBrowserIsVisible();
}

void QtBookmarkView::doRefreshView()
{
	setStyleSheet();
	m_widget->refreshStyle();
}

void QtBookmarkView::setStyleSheet()
{
	m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "bookmark_view/bookmark_view.css").c_str());
}

void QtBookmarkView::displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks)
{
	m_widget->displayBookmarks(bookmarks);
}

void QtBookmarkView::displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories)
{
	m_widget->displayBookmarkCreator(names, categories);
}

void QtBookmarkView::displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories)
{
	m_widget->displayBookmarkEditor(bookmark, categories);
}