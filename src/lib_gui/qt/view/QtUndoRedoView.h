#ifndef QT_UNDO_REDO_VIEW_H
#define QT_UNDO_REDO_VIEW_H

#include <memory>
#include <string>

#include "QtThreadedFunctor.h"
#include "QtUndoRedo.h"
#include "UndoRedoView.h"

class QtUndoRedoView: public UndoRedoView
{
public:
	QtUndoRedoView(ViewLayout* viewLayout);
	~QtUndoRedoView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// UndoRedo view implementation
	void setRedoButtonEnabled(bool enabled) override;
	void setUndoButtonEnabled(bool enabled) override;

	void updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex) override;

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QtUndoRedo* m_widget;
};

#endif	  // QT_UNDO_REDO_VIEW_H
