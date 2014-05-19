#ifndef QT_LAYOUT_H
#define QT_LAYOUT_H

#include <qwidget.h>
#include <qlayout.h>

#include "gui/GuiLayout.h"

class QtLayout: public GuiLayout
{
public:
	QtLayout();
	virtual ~QtLayout();

	virtual void setDirection(LayoutDirectionType direction);

	virtual void addChild(std::shared_ptr<GuiElement> element);
	virtual void removeChild(std::shared_ptr<GuiElement> element);

	virtual void setPosition(Vec2i position);
	virtual Vec2i getPosition() const;
	virtual void setSize(Vec2i size);
	virtual Vec2i getSize() const;

	virtual void setBackgroundColor(Colori color);

private:
	std::shared_ptr<QWidget> getWidget();

	QBoxLayout* m_layout;
};

# endif // QT_LAYOUT_H
