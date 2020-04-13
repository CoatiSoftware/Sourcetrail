#include "CodeView.h"

#include "../controller/CodeController.h"

const char* CodeView::VIEW_NAME = "Code";

CodeView::CodeView(ViewLayout* viewLayout): View(viewLayout) {}

CodeView::~CodeView() {}

std::string CodeView::getName() const
{
	return VIEW_NAME;
}

CodeController* CodeView::getController()
{
	return View::getController<CodeController>();
}
