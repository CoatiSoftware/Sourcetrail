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
	virtual void setText(const std::string& text);
	virtual void setMatch(const SearchMatch& match);
	virtual void setFocus();
	virtual void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

private:
	void doRefreshView();
	void doSetMatch(const SearchMatch& match);
	void doSetText(const std::string& text);
	void doSetFocus();
	void doSetAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

	void setStyleSheet();

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::string&> m_setTextFunctor;
	QtThreadedFunctor<const SearchMatch&> m_setMatchFunctor;
	QtThreadedFunctor<> m_setFocusFunctor;
	QtThreadedFunctor<const std::vector<SearchMatch>&> m_setAutocompletionListFunctor;

	QtSearchBar* m_widget;
};

# endif // QT_SEARCH_VIEW_H
