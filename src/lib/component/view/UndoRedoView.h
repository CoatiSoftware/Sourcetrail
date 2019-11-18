#ifndef UNDO_REDO_VIEW_H
#define UNDO_REDO_VIEW_H

#include <vector>

#include "View.h"
#include <vector>

class UndoRedoController;
struct SearchMatch;

class UndoRedoView: public View
{
public:
	UndoRedoView(ViewLayout* viewLayout);
	~UndoRedoView(void);

	virtual std::string getName() const;

	virtual void setRedoButtonEnabled(bool enabled) = 0;
	virtual void setUndoButtonEnabled(bool enabled) = 0;

	virtual void updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex) = 0;

protected:
	UndoRedoController* getController();
};

#endif	  // UNDO_REDO_VIEW_H
