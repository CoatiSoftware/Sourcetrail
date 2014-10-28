#ifndef QT_SEARCH_BOX_H
#define QT_SEARCH_BOX_H

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

	void setText(const std::string& text);
	void setFocus();
	void setAutocompletionList(const std::vector<SearchMatch>& autocompletionList);

	QAbstractItemView* getCompleterPopup();

private:
	QtSmartSearchBox* m_searchBox;
	QPushButton* m_searchButton;
	QPushButton* m_caseSensitiveButton;
};

#endif // QT_SEARCH_BOX_H
