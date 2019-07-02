#ifndef QT_SEARCH_VIEW_H
#define QT_SEARCH_VIEW_H

#include <vector>

#include "SearchView.h"
#include "QtThreadedFunctor.h"

class QtSearchBar;

class QtSearchView
	: public SearchView
{
public:
	QtSearchView(ViewLayout* viewLayout);
	~QtSearchView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// SearchView implementation
	std::wstring getQuery() const override;
	void setMatches(const std::vector<SearchMatch>& matches) override;
	void setFocus() override;
	void findFulltext() override;
	void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList) override;

private:
	void setStyleSheet();

	QtThreadedLambdaFunctor m_onQtThread;

	QtSearchBar* m_widget;
};

# endif // QT_SEARCH_VIEW_H
