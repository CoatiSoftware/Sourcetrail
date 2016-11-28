#include "qt/view/graphElements/QtGraphNodeData.h"

#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "data/graph/token_component/TokenComponentSignature.h"

QtGraphNodeData::QtGraphNodeData(const Node* data, const std::string& name, bool hasParent, bool childVisible, bool hasQualifier)
	: m_data(data)
	, m_childVisible(childVisible)
	, m_hasQualifier(hasQualifier)
{
	this->setAcceptHoverEvents(true);

	this->setName(name);

	std::string toolTip = data->getTypeString();
	if (!data->isDefined() && !data->isType(Node::NODE_UNDEFINED))
	{
		toolTip = "undefined " + toolTip;
	}
	else if (data->isImplicit())
	{
		toolTip = "implicit " + toolTip;
	}

	if (data->isType(Node::NODE_FUNCTION | Node::NODE_METHOD))
	{
		TokenComponentSignature* sig = data->getComponent<TokenComponentSignature>();
		if (sig)
		{
			toolTip += ": " + sig->getSignature();
		}
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
	if (m_isActive && !m_multipleActive)
	{
		MessageDeactivateEdge().dispatch();
		return;
	}

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
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleForNodeType(
		m_data->getType(), m_data->isExplicit(), m_isActive, m_isHovering, m_childVisible, m_hasQualifier);
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
