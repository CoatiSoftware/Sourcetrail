#include "component/view/CodeView.h"

#include "component/controller/CodeController.h"

CodeView::CodeSnippetParams::CodeSnippetParams()
	: locationFile("")
{
}

CodeView::CodeView(ViewLayout* viewLayout)
	: View(viewLayout, Vec2i(100, 100))
{
}

CodeView::~CodeView()
{
}

std::string CodeView::getName() const
{
	return "CodeView";
}

CodeController* CodeView::getController()
{
	return View::getController<CodeController>();
}
