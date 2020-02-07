#ifndef QT_AUTOCOMPLETION_LIST
#define QT_AUTOCOMPLETION_LIST

#include <memory>
#include <vector>

#include <QAbstractTableModel>
#include <QCompleter>
#include <QListView>
#include <QStyledItemDelegate>

#include "QtDeviceScaledPixmap.h"
#include "SearchMatch.h"

class QtAutocompletionModel: public QAbstractTableModel
{
	Q_OBJECT

public:
	QtAutocompletionModel(QObject* parent = 0);
	virtual ~QtAutocompletionModel();

	void setMatchList(const std::vector<SearchMatch>& matchList);

	virtual int rowCount(const QModelIndex& parent) const;
	virtual int columnCount(const QModelIndex& parent) const;

	virtual QVariant data(const QModelIndex& index, int role) const;

	const SearchMatch* getSearchMatchAt(int idx) const;

	QString longestText() const;
	QString longestSubText() const;
	QString longestType() const;

private:
	std::vector<SearchMatch> m_matchList;
};


class QtAutocompletionDelegate: public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit QtAutocompletionDelegate(QtAutocompletionModel* model, QObject* parent = 0);
	virtual ~QtAutocompletionDelegate();

	virtual void paint(
		QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

	void calculateCharSizes(QFont font);
	void resetCharSizes();

private:
	QtAutocompletionModel* m_model;

	QFont m_font1;
	QFont m_font2;

	float m_charWidth1;
	float m_charHeight1;
	float m_charWidth2;
	float m_charHeight2;

	QtDeviceScaledPixmap m_arrow;
};


class QtAutocompletionList: public QCompleter
{
	Q_OBJECT

signals:
	void matchHighlighted(const SearchMatch&);
	void matchActivated(const SearchMatch&);

public:
	QtAutocompletionList(QWidget* parent = 0);
	virtual ~QtAutocompletionList();

	void completeAt(QPoint pos, const std::vector<SearchMatch>& autocompletionList);

	const SearchMatch* getSearchMatchAt(int idx) const;

private slots:
	void onHighlighted(const QModelIndex& index);
	void onActivated(const QModelIndex& index);

private:
	std::shared_ptr<QtAutocompletionModel> m_model;
	std::shared_ptr<QtAutocompletionDelegate> m_delegate;
};

#endif	  // QT_AUTOCOMPLETION_LIST
