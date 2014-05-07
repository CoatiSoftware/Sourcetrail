#ifndef QT_CANVAS_H
#define QT_CANVAS_H

#include <qwidget.h>
#include <QtWidgets/QMainWindow>

#include "gui/GuiCanvas.h"

class QtCanvas: public GuiCanvas
{
public:
	QtCanvas();
	virtual ~QtCanvas();

	virtual void addChild(std::shared_ptr<GuiElement> element);
	virtual void removeChild(std::shared_ptr<GuiElement> element);

	virtual void setPosition(Vec2i position);
	virtual Vec2i getPosition() const;
	virtual void setSize(Vec2i size);
	virtual Vec2i getSize() const;

	virtual void setBackgroundColor(Colori color);

private:
	std::shared_ptr<QWidget> getWidget();
};

# endif // QT_CANVAS_H
