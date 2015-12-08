#ifndef QT_UNDO_REDO_H
#define QT_UNDO_REDO_H

#include <string>
#include <QFrame>


class QPushButton;

class QtUndoRedo
	: public QFrame
{
	Q_OBJECT

public:
	QtUndoRedo();
	~QtUndoRedo();

	void setRedoButtonEnabled(bool enabled);
	void setUndoButtonEnabled(bool enabled);

	void refreshStyle();

private slots:
    void undo();
    void redo();

private:
	QPushButton* m_undoButton;
	QPushButton* m_redoButton;

};

#endif // QT_UNDO_REDO_H
