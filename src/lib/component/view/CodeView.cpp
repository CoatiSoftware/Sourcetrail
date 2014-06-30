#include "component/view/CodeView.h"

CodeView::CodeView(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElement> rootElement)
	: View(viewManager, rootElement, Vec2i(100, 100))
{
}

CodeView::~CodeView()
{
}
