#ifndef QT_SEARCH_VIEW_H
#define QT_SEARCH_VIEW_H

#include <vector>

#include "component/view/SearchView.h"
#include "qt/element/QtSearchBar.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtSearchView: public SearchView
{
public:
	QtSearchView(ViewLayout* viewLayout);
	~QtSearchView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// SearchView implementation
	virtual std::string getQuery() const;
	virtual void setMatches(const std::vector<SearchMatch>& matches);
	virtual void setFocus();
	virtual void findFulltext();
	virtual void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

private:
	void doRefreshView();
	void doSetMatches(const std::vector<SearchMatch>& matches);
	void doSetFocus();
	void doFindFulltext();
	void doSetAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

	void setStyleSheet();

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::vector<SearchMatch>&> m_setMatchesFunctor;
	QtThreadedFunctor<> m_setFocusFunctor;
	QtThreadedFunctor<> m_findFulltextFunctor;
	QtThreadedFunctor<const std::vector<SearchMatch>&> m_setAutocompletionListFunctor;

	QtSearchBar* m_widget;
};

# endif // QT_SEARCH_VIEW_H
