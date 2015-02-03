#ifndef VIEW_FACTORY_H
#define VIEW_FACTORY_H

#include <memory>

class CodeView;
class GraphView;
class MainView;
class SearchView;
class StatusBarView;
class UndoRedoView;
class ViewLayout;

class ViewFactory
{
public:
	ViewFactory();
	virtual ~ViewFactory();

	virtual std::shared_ptr<MainView> createMainView() const = 0;
	virtual std::shared_ptr<CodeView> createCodeView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<GraphView> createGraphView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<SearchView> createSearchView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<StatusBarView> createStatusBarView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<UndoRedoView> createUndoRedoView(ViewLayout* viewLayout) const = 0;
};

#endif // VIEW_FACTORY_H
