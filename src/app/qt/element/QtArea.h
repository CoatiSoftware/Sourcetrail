#ifndef QT_AREA_H
#define QT_AREA_H

#include <qwidget.h>

#include "gui/GuiArea.h"

class QtArea: public GuiArea
{
public:
	QtArea();
	virtual ~QtArea();

	virtual void setPosition(Vec2i position);
	virtual Vec2i getPosition() const;
	virtual void setSize(Vec2i size);
	virtual Vec2i getSize() const;

	virtual void setBackgroundColor(Colori color);

private:
	std::shared_ptr<QWidget> getWidget();
};

# endif // QT_AREA_H
