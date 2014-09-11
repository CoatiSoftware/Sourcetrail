#ifndef QT_SEARCH_BOX_H
#define QT_SEARCH_BOX_H

#include <string>

#include <QAbstractItemView>
#include <QFrame>

#include "data/SearchIndex.h"

class QLineEdit;
class QPushButton;

class QtSearchBox
	: public QFrame
{
	Q_OBJECT

public:
	QtSearchBox();
	virtual ~QtSearchBox();

	void setText(const std::string& text);
	void setFocus();
	void setAutocompletionList(const std::vector<SearchIndex::SearchMatch>& autocompletionList);

	QAbstractItemView* getCompleterPopup();

private slots:
	void onSearchButtonClick();
	void onSearchQueryEdited(const QString& text);
	void onSearchQueryChanged(const QString& text);
	void onSearchCompletionHighlighted(const QModelIndex& index);
	void onSearchCompletionActivated(const QString& text);

private:
	QLineEdit* m_searchBox;
	QPushButton* m_searchButton;
	QPushButton* m_caseSensitiveButton;

	std::string m_query;
	std::string m_oldQuery;
	bool m_preventQueryChange;

	std::vector<SearchIndex::SearchMatch> m_matches;
};

#endif // QT_SEARCH_BOX_H
