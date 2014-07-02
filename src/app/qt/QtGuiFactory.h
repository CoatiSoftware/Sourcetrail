#ifndef QT_GUI_FACTORY_H
#define QT_GUI_FACTORY_H

#include "gui/GuiFactory.h"

class QtGuiFactory: public GuiFactory
{
public:
	QtGuiFactory();
	virtual ~QtGuiFactory();

	virtual std::shared_ptr<MainView> createMainView() const;
	virtual std::shared_ptr<CodeView> createCodeView(ViewLayout* viewLayout) const;
	virtual std::shared_ptr<GraphView> createGraphView(ViewLayout* viewLayout) const;
};

#endif // QT_GUI_FACTORY_H
