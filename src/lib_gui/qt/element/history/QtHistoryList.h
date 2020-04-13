#ifndef QT_HISTORY_LIST_H
#define QT_HISTORY_LIST_H

#include <QListWidget>

#include "../../../../lib/data/search/SearchMatch.h"

class QLabel;

class QtHistoryItem: public QWidget
{
	Q_OBJECT

public:
	QtHistoryItem(const SearchMatch& match, size_t index, bool isCurrent);

	QSize getSizeHint() const;

	const SearchMatch& getMatch() const;

	size_t index;

protected:
	// because changing font-weight within the stylesheet does not work for some reason
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);

private:
	QLabel* m_name;

	QWidget* m_indicator;
	std::string m_indicatorColor;
	std::string m_indicatorHoverColor;

	const SearchMatch m_match;
};


class QtHistoryListWidget: public QListWidget
{
public:
	QtHistoryListWidget(QWidget* parent = nullptr);

protected:
	void mouseReleaseEvent(QMouseEvent* event);
};


class QtHistoryList: public QWidget
{
	Q_OBJECT

signals:
	void closed();

public:
	QtHistoryList(const std::vector<SearchMatch>& history, size_t currentIndex);

	void showPopup(QPoint pos);

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void onItemClicked(QListWidgetItem* item);

private:
	QtHistoryListWidget* m_list;
	size_t m_currentIndex;
};

#endif	  // QT_HISTORY_LIST_H
