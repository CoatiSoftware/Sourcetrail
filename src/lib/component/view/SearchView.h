#ifndef SEARCH_VIEW_H
#define SEARCH_VIEW_H

#include "component/view/View.h"
#include "data/search/SearchMatch.h"

class SearchController;

class SearchView : public View
{
public:
	SearchView(ViewLayout* viewLayout);
	virtual ~SearchView();

	virtual std::string getName() const;

	virtual void setText(const std::string& s) = 0;
	virtual void setFocus() = 0;
	virtual void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList) = 0;

protected:
	SearchController* getController();
};

#endif // SEARCH_VIEW_H