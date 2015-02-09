#include "component/view/SearchView.h"

#include "component/controller/SearchController.h"

SearchView::SearchView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

SearchView::~SearchView()
{
}

std::string SearchView::getName() const
{
	return "SearchView";
}

SearchController* SearchView::getController()
{
	return View::getController<SearchController>();
}
