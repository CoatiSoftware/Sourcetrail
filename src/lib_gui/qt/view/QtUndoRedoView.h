#ifndef QT_UNDO_REDO_VIEW_H
#define QT_UNDO_REDO_VIEW_H

#include <memory>
#include <string>

#include "component/view/UndoRedoView.h"
#include "qt/element/QtUndoRedo.h"
#include "qt/utility/QtThreadedFunctor.h"

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

private:
	void doRefreshView();

    void doSetRedoButtonEnabled(bool enabled);
    void doSetUndoButtonEnabled(bool enabled);

    QtThreadedFunctor<void> m_refreshFunctor;
    QtThreadedFunctor<bool> m_setRedoButtonEnabledFunctor;
    QtThreadedFunctor<bool> m_setUndoButtonEnabledFunctor;

	void setStyleSheet();
	QtUndoRedo* m_widget;
};

#endif // !QT_UNDO_REDO_VIEW_H
