#ifndef QT_AUTOCOMPLETION_LIST
#define QT_AUTOCOMPLETION_LIST

#include <memory>
#include <vector>

#include <QAbstractTableModel>
#include <QCompleter>
#include <QItemDelegate>
#include <QListView>

#include "data/search/SearchMatch.h"

class QtAutocompletionModel
	: public QAbstractTableModel
{
	Q_OBJECT

public:
	QtAutocompletionModel(const std::vector<SearchMatch>& matchList, QObject* parent = 0);
	virtual ~QtAutocompletionModel();

	virtual int rowCount(const QModelIndex& parent) const;
	virtual int columnCount(const QModelIndex& parent) const;

	virtual QVariant data(const QModelIndex& index, int role) const;

	const SearchMatch* getSearchMatchAt(int idx) const;

private:
	const std::vector<SearchMatch>& m_matchList;
};


class QtAutocompletionDelegate
	: public QItemDelegate
{
public:
	explicit QtAutocompletionDelegate(QObject* parent = 0);
	virtual ~QtAutocompletionDelegate();

	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


class QtAutocompletionList
	: public QCompleter
{
	Q_OBJECT

signals:
	void matchHighlighted(const SearchMatch&);
	void matchActivated(const SearchMatch&);

public:
	QtAutocompletionList(const std::vector<SearchMatch>& autocompletionList, QWidget* parent = 0);
	virtual ~QtAutocompletionList();

	const SearchMatch* getSearchMatchAt(int idx) const;

private slots:
	void onHighlighted(const QModelIndex& index);
	void onActivated(const QModelIndex& index);

private:
	std::shared_ptr<QtAutocompletionModel> m_model;
	std::shared_ptr<QtAutocompletionDelegate> m_delegate;
};

#endif // QT_AUTOCOMPLETION_LIST
