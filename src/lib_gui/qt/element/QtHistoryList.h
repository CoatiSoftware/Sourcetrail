#ifndef QT_HISTORY_LIST_H
#define QT_HISTORY_LIST_H

#include <QListWidget>

class QLabel;
struct SearchMatch;

class QtHistoryItem
	: public QWidget
{
	Q_OBJECT

public:
	QtHistoryItem(const SearchMatch& match, size_t index, bool isCurrent);

	QSize getSizeHint() const;

	size_t index;

private:
	QLabel* m_name;
};



class QtHistoryList
	: public QListWidget
{
	Q_OBJECT

public:
	QtHistoryList(const std::vector<SearchMatch>& history, size_t currentIndex);

	void showPopup(QPoint pos);

private slots:
	void onItemClicked(QListWidgetItem *item);

private:
	size_t m_currentIndex;
};

#endif // QT_HISTORY_LIST_H
