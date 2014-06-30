#ifndef GUI_ELEMENT_FACTORY_H
#define GUI_ELEMENT_FACTORY_H

#include <memory>

class CodeView;

class GuiArea;
class GuiCanvas;
class GuiLayout;
class GuiWindow;

class ViewManager;

class GuiElementFactory
{
public:
	virtual ~GuiElementFactory();

	virtual std::shared_ptr<GuiArea> createArea() const = 0;
	virtual std::shared_ptr<GuiCanvas> createCanvas(std::shared_ptr<GuiWindow> window) const = 0;
	virtual std::shared_ptr<GuiLayout> createLayout() const = 0;

	virtual std::shared_ptr<CodeView> createCodeView(std::shared_ptr<ViewManager> viewManager) const = 0;
};

#endif // GUI_ELEMENT_FACTORY_H
