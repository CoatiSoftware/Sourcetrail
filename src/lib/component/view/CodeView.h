#ifndef CODE_VIEW_H
#define CODE_VIEW_H

#include <string>

#include "component/view/View.h"

class GuiElementFactory;

class CodeView: public View
{
public:
	CodeView(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElement> rootElement);
	virtual ~CodeView();

	virtual void addCodeSnippet(std::string str) = 0;
	virtual void clearCodeSnippets() = 0;
};

#endif // CODE_VIEW_H
