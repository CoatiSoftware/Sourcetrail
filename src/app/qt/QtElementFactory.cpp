#include "qt/QtElementFactory.h"

#include "component/view/CodeView.h"
#include "qt/element/QtArea.h"
#include "qt/element/QtCanvas.h"
#include "qt/element/QtLayout.h"
#include "qt/view/QtCodeView.h"
#include "qt/QtWindow.h"

QtElementFactory::QtElementFactory()
{
}

QtElementFactory::~QtElementFactory()
{
}

std::shared_ptr<GuiArea> QtElementFactory::createArea() const
{
	return std::make_shared<QtArea>();
}

std::shared_ptr<GuiCanvas> QtElementFactory::createCanvas(std::shared_ptr<GuiWindow> window) const
{
	std::shared_ptr<GuiCanvas> canvas = std::make_shared<QtCanvas>();
	window->setCanvas(canvas);
	return canvas;
}

std::shared_ptr<GuiLayout> QtElementFactory::createLayout() const
{
	return std::make_shared<QtLayout>();
}

std::shared_ptr<CodeView> QtElementFactory::createCodeView(std::shared_ptr<ViewManager> viewManager) const
{
	return std::make_shared<QtCodeView>(viewManager);
}
