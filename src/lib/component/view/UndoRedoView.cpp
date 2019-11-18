#include "UndoRedoView.h"

#include "UndoRedoController.h"

UndoRedoView::UndoRedoView(ViewLayout* viewLayout): View(viewLayout) {}

UndoRedoView::~UndoRedoView() {}

std::string UndoRedoView::getName() const
{
	return "UndoRedoView";
}

UndoRedoController* UndoRedoView::getController()
{
	return View::getController<UndoRedoController>();
}
