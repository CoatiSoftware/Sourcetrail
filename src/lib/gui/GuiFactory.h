#ifndef GUI_FACTORY_H
#define GUI_FACTORY_H

#include <memory>

class CodeView;
class GraphView;
class MainView;
class SearchView;
class ViewLayout;

class GuiFactory
{
public:
	GuiFactory();
	virtual ~GuiFactory();

	virtual std::shared_ptr<MainView> createMainView() const = 0;
	virtual std::shared_ptr<CodeView> createCodeView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<GraphView> createGraphView(ViewLayout* viewLayout) const = 0;
	virtual std::shared_ptr<SearchView> createSearchView(ViewLayout* viewLayout) const = 0;
};

#endif // GUI_FACTORY_H
