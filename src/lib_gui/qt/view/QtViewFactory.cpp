#include "qt/view/QtViewFactory.h"

#include "component/view/GraphViewStyle.h"
#include "qt/view/QtCodeView.h"
#include "qt/view/QtCompositeView.h"
#include "qt/view/QtDialogView.h"
#include "qt/view/QtErrorView.h"
#include "qt/view/QtGraphView.h"
#include "qt/view/QtGraphViewStyleImpl.h"
#include "qt/view/QtLogView.h"
#include "qt/view/QtMainView.h"
#include "qt/view/QtRefreshView.h"
#include "qt/view/QtSearchView.h"
#include "qt/view/QtStatusBarView.h"
#include "qt/view/QtTabbedView.h"
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

std::shared_ptr<CompositeView> QtViewFactory::createCompositeView(
	ViewLayout* viewLayout, CompositeView::CompositeDirection direction, const std::string& name
) const {
	std::shared_ptr<CompositeView> ptr = std::make_shared<QtCompositeView>(viewLayout, direction, name);
	ptr->init();
	ptr->addToLayout();
	return ptr;
}

std::shared_ptr<TabbedView> QtViewFactory::createTabbedView(ViewLayout* viewLayout, const std::string& name) const
{
	std::shared_ptr<TabbedView> ptr = std::make_shared<QtTabbedView>(viewLayout, name);
	ptr->init();
	ptr->addToLayout();
	return ptr;
}

std::shared_ptr<CodeView> QtViewFactory::createCodeView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtCodeView>(viewLayout);
}

std::shared_ptr<ErrorView> QtViewFactory::createErrorView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtErrorView>(viewLayout);
}

std::shared_ptr<LogView> QtViewFactory::createLogView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtLogView>(viewLayout);
}

std::shared_ptr<GraphView> QtViewFactory::createGraphView(ViewLayout* viewLayout) const
{
	GraphViewStyle::setImpl(std::make_shared<QtGraphViewStyleImpl>());
	return View::createInitAndAddToLayout<QtGraphView>(viewLayout);
}

std::shared_ptr<RefreshView> QtViewFactory::createRefreshView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtRefreshView>(viewLayout);
}

std::shared_ptr<SearchView> QtViewFactory::createSearchView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtSearchView>(viewLayout);
}

std::shared_ptr<StatusBarView> QtViewFactory::createStatusBarView(ViewLayout* viewLayout) const
{
	return View::createAndInit<QtStatusBarView>(viewLayout);
}

std::shared_ptr<UndoRedoView> QtViewFactory::createUndoRedoView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtUndoRedoView>(viewLayout);
}

std::shared_ptr<DialogView> QtViewFactory::createDialogView(ViewLayout* viewLayout, StorageAccess* storageAccess) const
{
	return std::make_shared<QtDialogView>(dynamic_cast<QtMainView*>(viewLayout)->getMainWindow(), storageAccess);
}
