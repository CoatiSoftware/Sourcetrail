#ifndef QT_UNDO_REDO_VIEW_H
#define QT_UNDO_REDO_VIEW_H

#include <memory>
#include <string>

#include "UndoRedoView.h"
#include "QtUndoRedo.h"
#include "QtThreadedFunctor.h"

class QtUndoRedoView : public UndoRedoView
{
public:
	QtUndoRedoView(ViewLayout* viewLayout);
	~QtUndoRedoView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// UndoRedo view implementation
	virtual void setRedoButtonEnabled(bool enabled);
	virtual void setUndoButtonEnabled(bool enabled);

	virtual void updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex);

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QtUndoRedo* m_widget;
};

#endif // QT_UNDO_REDO_VIEW_H
