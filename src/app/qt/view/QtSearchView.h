#ifndef QT_SEARCH_VIEW_H
#define QT_SEARCH_VIEW_H

#include <vector>

#include "component/view/SearchView.h"
#include "qt/utility/QtThreadedFunctor.h"

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
	virtual void setText(const std::string& s);
	virtual void setAutocompletionList(const std::vector<std::string>& autocompletionList);

private:
	void onSearchButtonClick();

	void doSetText(const std::string& s);
	void doSetAutocompletionList(const std::vector<std::string>& autocompletionList);

	QtEditBox* m_searchBox;
	QtButton* m_searchButton;
	QtThreadedFunctor<const std::string&> m_setTextFunctor;
	QtThreadedFunctor<const std::vector<std::string>&> m_setAutocompletionListFunctor;
};

# endif // QT_SEARCH_VIEW_H
