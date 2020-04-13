#include "BookmarkView.h"

#include "../controller/BookmarkController.h"

BookmarkView::BookmarkView(ViewLayout* viewLayout): View(viewLayout) {}

std::string BookmarkView::getName() const
{
	return "BookmarkView";
}

BookmarkController* BookmarkView::getController()
{
	return View::getController<BookmarkController>();
}
