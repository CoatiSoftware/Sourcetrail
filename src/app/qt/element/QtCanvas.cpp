#include "qt/element/QtCanvas.h"

#include <qgroupbox.h>
#include <qlayout.h>

#include "qt/QtWidgetWrapper.h"
#include "utility/utilityQt.h"

QtCanvas::QtCanvas()
	: GuiCanvas(std::make_shared<QtWidgetWrapper>(std::make_shared<QGroupBox>()))
{
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom); // gets deleted by qt
	getWidget()->setLayout(layout);
}

QtCanvas::~QtCanvas()
{
}

void QtCanvas::addChild(std::shared_ptr<GuiElement> element)
{
	getWidget()->layout()->addWidget(QtWidgetWrapper::getWidgetOfElement(element).get());
}

void QtCanvas::removeChild(std::shared_ptr<GuiElement> element)
{
	std::shared_ptr<QWidget> widgetToRemove = QtWidgetWrapper::getWidgetOfElement(element);
	getWidget()->layout()->removeWidget(widgetToRemove.get());
	widgetToRemove->setGeometry(0, 0, 0, 0);
}

void QtCanvas::setPosition(Vec2i position)
{
}

Vec2i QtCanvas::getPosition() const
{
	return Vec2i();
}

void QtCanvas::setSize(Vec2i size)
{
}

Vec2i QtCanvas::getSize() const
{
	return Vec2i();
}

void QtCanvas::setBackgroundColor(Colori color)
{
	utility::setWidgetBackgroundColor(getWidget(), color);
}

std::shared_ptr<QWidget> QtCanvas::getWidget()
{
	return QtWidgetWrapper::getWidgetOfElement(this);
}

