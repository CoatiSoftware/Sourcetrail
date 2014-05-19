#ifndef QT_ELEMENT_FACTORY_H
#define QT_ELEMENT_FACTORY_H

#include "gui/GuiElementFactory.h"

class QtElementFactory: public GuiElementFactory
{
public:
	QtElementFactory();
	virtual ~QtElementFactory();

	virtual std::shared_ptr<GuiArea> createArea() const;
	virtual std::shared_ptr<GuiCanvas> createCanvas(std::shared_ptr<GuiWindow> window) const;
	virtual std::shared_ptr<GuiLayout> createLayout() const;
};

# endif // QT_ELEMENT_FACTORY_H
