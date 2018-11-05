#include "QtViewFactory.h"

#include "GraphViewStyle.h"
#include "QtBookmarkButtonsView.h"
#include "QtBookmarkView.h"
#include "QtCodeView.h"
#include "QtCompositeView.h"
#include "QtDialogView.h"
#include "QtErrorView.h"
#include "QtGraphView.h"
#include "QtGraphViewStyleImpl.h"
#include "QtMainView.h"
#include "QtRefreshView.h"
#include "QtScreenSearchView.h"
#include "QtSearchView.h"
#include "QtStatusBarView.h"
#include "QtStatusView.h"
#include "QtTabbedView.h"
#include "QtTabsView.h"
#include "QtTooltipView.h"
#include "QtUndoRedoView.h"

QtViewFactory::QtViewFactory()
{
}

std::shared_ptr<MainView> QtViewFactory::createMainView(StorageAccess* storageAccess) const
{
	return std::make_shared<QtMainView>(this, storageAccess);
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

std::shared_ptr<BookmarkButtonsView> QtViewFactory::createBookmarkButtonsView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtBookmarkButtonsView>(viewLayout);
}

std::shared_ptr<BookmarkView> QtViewFactory::createBookmarkView(ViewLayout* viewLayout) const
{
	return View::createAndInit<QtBookmarkView>(viewLayout);
}

std::shared_ptr<CodeView> QtViewFactory::createCodeView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtCodeView>(viewLayout);
}

std::shared_ptr<ErrorView> QtViewFactory::createErrorView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtErrorView>(viewLayout);
}

std::shared_ptr<StatusView> QtViewFactory::createStatusView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtStatusView>(viewLayout);
}

std::shared_ptr<GraphView> QtViewFactory::createGraphView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtGraphView>(viewLayout);
}

std::shared_ptr<RefreshView> QtViewFactory::createRefreshView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtRefreshView>(viewLayout);
}

std::shared_ptr<ScreenSearchView> QtViewFactory::createScreenSearchView(ViewLayout* viewLayout) const
{
	return View::createAndInit<QtScreenSearchView>(viewLayout);
}

std::shared_ptr<SearchView> QtViewFactory::createSearchView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtSearchView>(viewLayout);
}

std::shared_ptr<StatusBarView> QtViewFactory::createStatusBarView(ViewLayout* viewLayout) const
{
	return View::createAndInit<QtStatusBarView>(viewLayout);
}

std::shared_ptr<TabsView> QtViewFactory::createTabsView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtTabsView>(viewLayout);
}

std::shared_ptr<TooltipView> QtViewFactory::createTooltipView(ViewLayout* viewLayout) const
{
	return View::createAndInit<QtTooltipView>(viewLayout);
}

std::shared_ptr<UndoRedoView> QtViewFactory::createUndoRedoView(ViewLayout* viewLayout) const
{
	return View::createInitAndAddToLayout<QtUndoRedoView>(viewLayout);
}

std::shared_ptr<DialogView> QtViewFactory::createDialogView(
	ViewLayout* viewLayout, DialogView::UseCase useCase, StorageAccess* storageAccess) const
{
	return std::make_shared<QtDialogView>(dynamic_cast<QtMainView*>(viewLayout)->getMainWindow(), useCase, storageAccess);
}

std::shared_ptr<GraphViewStyleImpl> QtViewFactory::createGraphStyleImpl() const
{
	return std::make_shared<QtGraphViewStyleImpl>();
}
