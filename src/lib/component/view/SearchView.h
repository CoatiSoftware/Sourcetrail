#ifndef SEARCH_VIEW_H
#define SEARCH_VIEW_H

#include "component/view/View.h"

class SearchController;

class SearchView : public View
{
public:
	SearchView(ViewLayout* viewLayout);
	virtual ~SearchView();

	virtual std::string getName() const;

	virtual void setText(const std::string& s) const = 0;

protected:
	SearchController* getController();
};

#endif // SEARCH_VIEW_H