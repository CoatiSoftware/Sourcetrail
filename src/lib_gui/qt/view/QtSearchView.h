#ifndef QT_SEARCH_VIEW_H
#define QT_SEARCH_VIEW_H

#include <vector>

#include "component/view/SearchView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtSearchBar;

class QtSearchView
	: public SearchView
{
public:
	QtSearchView(ViewLayout* viewLayout);
	~QtSearchView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// SearchView implementation
	virtual std::wstring getQuery() const;
	virtual void setMatches(const std::vector<SearchMatch>& matches);
	virtual void setFocus();
	virtual void findFulltext();
	virtual void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

private:
	void setStyleSheet();

	QtThreadedLambdaFunctor m_onQtThread;

	QtSearchBar* m_widget;
};

# endif // QT_SEARCH_VIEW_H
