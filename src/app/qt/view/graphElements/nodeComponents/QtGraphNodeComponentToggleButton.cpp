#include "QtGraphNodeComponentToggleButton.h"

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>

#include "qt/view/graphElements/QtGraphNode.h"

QtGraphNodeComponentToggleButton::QtGraphNodeComponentToggleButton(const std::weak_ptr<QtGraphNode>& graphNode)
	: QtGraphNodeComponent(graphNode)
{
	this->setRect(0, 0, 10, 10);
	this->setPos(0, 0);
	QBrush brush(Qt::darkGray);
	this->setBrush(brush);

	std::shared_ptr<QtGraphNode> node = graphNode.lock();
	if(node != NULL)
	{
		this->setParentItem(node.get());
	}
}

QtGraphNodeComponentToggleButton::~QtGraphNodeComponentToggleButton()
{
}

void QtGraphNodeComponentToggleButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	std::shared_ptr<QtGraphNode> node = m_graphNode.lock();
	if(node != NULL)
	{
		if(node->contentIsHidden())
		{
			node->showContent();
		}
		else
		{
			node->hideContent();
		}
	}
}
