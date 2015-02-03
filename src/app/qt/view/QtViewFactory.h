#ifndef QT_VIEW_FACTORY_H
#define QT_VIEW_FACTORY_H

#include "component/view/ViewFactory.h"

class QtViewFactory: public ViewFactory
{
public:
	QtViewFactory();
	virtual ~QtViewFactory();

	virtual std::shared_ptr<MainView> createMainView() const;
	virtual std::shared_ptr<CodeView> createCodeView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<GraphView> createGraphView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<RefreshView> createRefreshView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<SearchView> createSearchView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<StatusBarView> createStatusBarView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<UndoRedoView> createUndoRedoView(ViewLayout* viewLayout) const;
};

#endif // QT_VIEW_FACTORY_H
