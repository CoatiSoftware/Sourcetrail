#include "qt/QtGuiFactory.h"

#include "qt/view/QtCodeView.h"
#include "qt/view/QtGraphView.h"
#include "qt/view/QtMainView.h"
#include "qt/view/QtSearchView.h"

QtGuiFactory::QtGuiFactory()
{
}

QtGuiFactory::~QtGuiFactory()
{
}

std::shared_ptr<MainView> QtGuiFactory::createMainView() const
{
	return std::make_shared<QtMainView>();
}

std::shared_ptr<CodeView> QtGuiFactory::createCodeView(ViewLayout* viewLayout) const
{
	return View::create<QtCodeView>(viewLayout);
}

std::shared_ptr<GraphView> QtGuiFactory::createGraphView(ViewLayout* viewLayout) const
{
	return View::create<QtGraphView>(viewLayout);
}

std::shared_ptr<SearchView> QtGuiFactory::createSearchView(ViewLayout* viewLayout) const
{
	return View::create<QtSearchView>(viewLayout);
}
