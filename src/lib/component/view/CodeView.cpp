#include "component/view/CodeView.h"

#include "component/controller/CodeController.h"

CodeView::CodeView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

CodeView::~CodeView()
{
}

std::string CodeView::getName() const
{
	return "Code";
}

CodeController* CodeView::getController()
{
	return View::getController<CodeController>();
}
