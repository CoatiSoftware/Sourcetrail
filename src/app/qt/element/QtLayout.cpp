#include "qt/element/QtLayout.h"

#include <qgroupbox.h>
#include <qlayout.h>

#include "qt/QtWidgetWrapper.h"
#include "utility/utilityQt.h"

QtLayout::QtLayout()
	: GuiLayout(std::make_shared<QtWidgetWrapper>(std::make_shared<QGroupBox>()))
{
	m_layout = new QBoxLayout(QBoxLayout::TopToBottom); // gets deleted by qt
	getWidget()->setLayout(m_layout);
}

QtLayout::~QtLayout()
{
}

void QtLayout::setDirection(LayoutDirectionType direction)
{
	switch (direction)
	{
	case LAYOUT_DIRECTION_HORIZONTAL:
		m_layout->setDirection(QBoxLayout::LeftToRight);
	case LAYOUT_DIRECTION_VERTICAL:
		m_layout->setDirection(QBoxLayout::TopToBottom);
	}
	getWidget()->setLayout(m_layout);
}

void QtLayout::addChild(std::shared_ptr<GuiElement> element)
{
	m_layout->addWidget(QtWidgetWrapper::getWidgetOfElement(element).get());
}

void QtLayout::removeChild(std::shared_ptr<GuiElement> element)
{
	std::shared_ptr<QWidget> widgetToRemove = QtWidgetWrapper::getWidgetOfElement(element);
	m_layout->removeWidget(widgetToRemove.get());
	widgetToRemove->setGeometry(0, 0, 0, 0);
}

void QtLayout::setPosition(Vec2i position)
{
}

Vec2i QtLayout::getPosition() const
{
	return Vec2i();
}

void QtLayout::setSize(Vec2i size)
{
}

Vec2i QtLayout::getSize() const
{
	return Vec2i();
}

void QtLayout::setBackgroundColor(Colori color)
{
	utility::setWidgetBackgroundColor(getWidget(), color);
}

std::shared_ptr<QWidget> QtLayout::getWidget()
{
	return QtWidgetWrapper::getWidgetOfElement(this);
}
