#include "qt/view/graphElements/QtGraphNodeData.h"

#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

QtGraphNodeData::QtGraphNodeData(const Node* data, bool hasParent, bool childVisible)
	: m_data(data)
	, m_childVisible(childVisible)
{
	this->setAcceptHoverEvents(true);

	if (!hasParent)
	{
		this->setName(data->getFullName());
	}
	else
	{
		this->setName(data->getName());
	}

	std::string toolTip = data->getTypeString();
	if (!data->isDefined() && !data->isType(Node::NODE_UNDEFINED))
	{
		toolTip = "undefined " + toolTip;
	}

	this->setToolTip(QString::fromStdString(toolTip));
}

QtGraphNodeData::~QtGraphNodeData()
{
}

const Node* QtGraphNodeData::getData() const
{
	return m_data;
}

bool QtGraphNodeData::isDataNode() const
{
	return true;
}

Id QtGraphNodeData::getTokenId() const
{
	return m_data->getId();
}

void QtGraphNodeData::onClick()
{
	MessageActivateNodes message;
	message.addNode(m_data->getId(), m_data->getType(), m_data->getNameHierarchy());
	message.dispatch();
}

void QtGraphNodeData::moved(const Vec2i& oldPosition)
{
	QtGraphNode::moved(oldPosition);

	MessageGraphNodeMove(m_data->getId(), getPosition() - oldPosition).dispatch();
}

void QtGraphNodeData::updateStyle()
{
	GraphViewStyle::NodeStyle style =
		GraphViewStyle::getStyleForNodeType(m_data->getType(), m_data->isDefined(), m_isActive, m_isHovering, m_childVisible);
	setStyle(style);
}

void QtGraphNodeData::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusIn(std::vector<Id>(1, m_data->getId())).dispatch();
}

void QtGraphNodeData::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusOut(std::vector<Id>(1, m_data->getId())).dispatch();
}
