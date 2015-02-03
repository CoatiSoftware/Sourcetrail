#include "qt/view/QtViewFactory.h"

#include "qt/view/QtCodeView.h"
#include "qt/view/QtGraphView.h"
#include "qt/view/QtMainView.h"
#include "qt/view/QtRefreshView.h"
#include "qt/view/QtSearchView.h"
#include "qt/view/QtStatusBarView.h"
#include "qt/view/QtUndoRedoView.h"

QtViewFactory::QtViewFactory()
{
}

QtViewFactory::~QtViewFactory()
{
}

std::shared_ptr<MainView> QtViewFactory::createMainView() const
{
	return std::make_shared<QtMainView>();
}

std::shared_ptr<CodeView> QtViewFactory::createCodeView(ViewLayout* viewLayout) const
{
	return View::create<QtCodeView>(viewLayout);
}

std::shared_ptr<GraphView> QtViewFactory::createGraphView(ViewLayout* viewLayout) const
{
	return View::create<QtGraphView>(viewLayout);
}

std::shared_ptr<RefreshView> QtViewFactory::createRefreshView(ViewLayout* viewLayout) const
{
	return View::create<QtRefreshView>(viewLayout);
}

std::shared_ptr<SearchView> QtViewFactory::createSearchView(ViewLayout* viewLayout) const
{
	return View::create<QtSearchView>(viewLayout);
}

std::shared_ptr<StatusBarView> QtViewFactory::createStatusBarView(ViewLayout* viewLayout) const
{
	return View::createAndDontAddToLayout<QtStatusBarView>(viewLayout);
}

std::shared_ptr<UndoRedoView> QtViewFactory::createUndoRedoView(ViewLayout* viewLayout) const
{
	return View::create<QtUndoRedoView>(viewLayout);
}
