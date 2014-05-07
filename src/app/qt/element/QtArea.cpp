#include "qt/element/QtArea.h"

#include <qframe.h>

#include "qt/QtWidgetWrapper.h"
#include "utility/utilityQt.h"

QtArea::QtArea()
	: GuiArea(std::make_shared<QtWidgetWrapper>(std::make_shared<QFrame>()))
{
}

QtArea::~QtArea()
{
}

void QtArea::setPosition(Vec2i position)
{
}

Vec2i QtArea::getPosition() const
{
	return Vec2i();
}

void QtArea::setSize(Vec2i size)
{
}

Vec2i QtArea::getSize() const
{
	return Vec2i();
}

void QtArea::setBackgroundColor(Colori color)
{
	utility::setWidgetBackgroundColor(getWidget(), color);
}

std::shared_ptr<QWidget> QtArea::getWidget()
{
	return QtWidgetWrapper::getWidgetOfElement(this);
}
