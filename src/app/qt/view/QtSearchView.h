#ifndef QT_SEARCH_VIEW_H
#define QT_SEARCH_VIEW_H

#include "component/view/SearchView.h"

class QtEditBox;
class QtButton;

class QtSearchView: public SearchView
{
public:
	QtSearchView(ViewLayout* viewLayout);
	~QtSearchView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

	// SearchView implementation
	virtual void setText(const std::string& s) const;

private:
	void onSeachButtonClick();

	QtEditBox* m_searchBox;
	QtButton* m_searchButton;
};

# endif // QT_SEARCH_VIEW_H
