#ifndef DUMMY_VIEW_H
#define DUMMY_VIEW_H

#include "component/view/View.h"

class GuiElementFactory;

class DummyView: public View
{
public:
	DummyView(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElementFactory> guiElementFactory);
	virtual ~DummyView();

private:
	std::shared_ptr<GuiElementFactory> m_guiElementFactory;
};


#endif // DUMMY_VIEW_H
