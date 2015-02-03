#include "component/view/UndoRedoView.h"

#include "component/controller/UndoRedoController.h"

UndoRedoView::UndoRedoView(ViewLayout* viewLayout)
	: View(viewLayout, Vec2i(100,100))
{
}

UndoRedoView::~UndoRedoView()
{
}

std::string UndoRedoView::getName() const
{
	return "UndoRedoView";
}

UndoRedoController* UndoRedoView::getController()
{
	return View::getController<UndoRedoController>();
}
