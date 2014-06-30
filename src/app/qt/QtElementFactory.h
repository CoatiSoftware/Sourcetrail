#ifndef QT_ELEMENT_FACTORY_H
#define QT_ELEMENT_FACTORY_H

#include "gui/GuiElementFactory.h"

class CodeView;
class ViewManager;

class QtElementFactory: public GuiElementFactory
{
public:
	QtElementFactory();
	virtual ~QtElementFactory();

	virtual std::shared_ptr<GuiArea> createArea() const;
	virtual std::shared_ptr<GuiCanvas> createCanvas(std::shared_ptr<GuiWindow> window) const;
	virtual std::shared_ptr<GuiLayout> createLayout() const;

	virtual std::shared_ptr<CodeView> createCodeView(std::shared_ptr<ViewManager> viewManager) const;
};

# endif // QT_ELEMENT_FACTORY_H
