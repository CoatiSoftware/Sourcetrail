#ifndef QT_UNDO_REDO_H
#define QT_UNDO_REDO_H

#include <string>
#include <QFrame>

#include "data/search/SearchMatch.h"
#include "utility/TimeStamp.h"

class QPushButton;
class QtHistoryList;

class QtUndoRedo
	: public QFrame
{
	Q_OBJECT

public:
	QtUndoRedo();
	~QtUndoRedo();

	void setRedoButtonEnabled(bool enabled);
	void setUndoButtonEnabled(bool enabled);

	void updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex);

	void refreshStyle();

private slots:
	void buttonPressed();

	void undoReleased();
	void redoReleased();

	void showHistory();
	void hidHistory();

private:
	QPushButton* m_undoButton;
	QPushButton* m_historyButton;
	QPushButton* m_redoButton;

	std::vector<SearchMatch> m_history;
	size_t m_currentIndex;

	bool m_pressed;

	QtHistoryList* m_historyList;
	TimeStamp m_historyHiddenAt;
};

#endif // QT_UNDO_REDO_H
