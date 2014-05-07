#include "component/view/DummyView.h"

#include "gui/GuiArea.h"
#include "gui/GuiElementFactory.h"

DummyView::DummyView(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElementFactory> guiElementFactory)
	: View(viewManager, guiElementFactory->createArea(), Vec2i(100, 100))
	, m_guiElementFactory(guiElementFactory)
{
	getRootElement()->setBackgroundColor(0, 255, 0, 255);
}

DummyView::~DummyView()
{
}
