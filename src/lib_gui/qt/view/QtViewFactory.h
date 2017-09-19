#ifndef QT_VIEW_FACTORY_H
#define QT_VIEW_FACTORY_H

#include "component/view/ViewFactory.h"

class QtViewFactory: public ViewFactory
{
public:
	QtViewFactory();
	virtual ~QtViewFactory();

	virtual std::shared_ptr<MainView> createMainView() const;
	virtual std::shared_ptr<CompositeView> createCompositeView(
		ViewLayout* viewLayout, CompositeView::CompositeDirection direction, const std::string& name) const;
	virtual std::shared_ptr<TabbedView> createTabbedView(ViewLayout* viewLayout, const std::string& name) const;

	virtual std::shared_ptr<BookmarkView> createBookmarkView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<CodeView> createCodeView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<ErrorView> createErrorView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<GraphView> createGraphView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<LogView> createLogView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<RefreshView> createRefreshView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<ScreenSearchView> createScreenSearchView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<SearchView> createSearchView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<StatusBarView> createStatusBarView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<StatusView> createStatusView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<TooltipView> createTooltipView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<UndoRedoView> createUndoRedoView(ViewLayout* viewLayout) const;

	virtual std::shared_ptr<DialogView> createDialogView(ViewLayout* viewLayout, StorageAccess* storageAccess) const;
};

#endif // QT_VIEW_FACTORY_H
