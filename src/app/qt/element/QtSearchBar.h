#ifndef QT_SEARCH_BAR_H
#define QT_SEARCH_BAR_H

#include <string>

#include <QAbstractItemView>
#include <QFrame>

#include "data/search/SearchMatch.h"

class QPushButton;
class QtSmartSearchBox;

class QtSearchBar
	: public QFrame
{
	Q_OBJECT

public:
	QtSearchBar();
	virtual ~QtSearchBar();

	virtual QSize sizeHint() const;

	void setMatches(const std::vector<SearchMatch>& matches);
	void setFocus();
	void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

	QAbstractItemView* getCompleterPopup();

	void refreshStyle();

private:
	QWidget* m_searchBoxContainer; // used for correct clipping inside the search box
	QtSmartSearchBox* m_searchBox;
	QPushButton* m_searchButton;
};

#endif // QT_SEARCH_BAR_H
