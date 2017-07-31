#include "qt/view/graphElements/QtGraphNodeData.h"

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"

#include "data/graph/token_component/TokenComponentFilePath.h"

QtGraphNodeData::QtGraphNodeData(const Node* data, const std::string& name, bool hasParent, bool childVisible, bool hasQualifier)
	: m_data(data)
	, m_childVisible(childVisible)
	, m_hasQualifier(hasQualifier)
{
	this->setAcceptHoverEvents(true);

	this->setName(name);
}

QtGraphNodeData::~QtGraphNodeData()
{
}

const Node* QtGraphNodeData::getData() const
{
	return m_data;
}

FilePath QtGraphNodeData::getFilePath() const
{
	if (m_data->isType(Node::NODE_FILE))
	{
		return m_data->getComponent<TokenComponentFilePath>()->getFilePath();
	}

	return FilePath();
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
		MessageDeactivateEdge(true).dispatch();
		return;
	}

	FilePath path = getFilePath();

	MessageActivateNodes message;
	message.addNode(m_data->getId(), path.empty() ? m_data->getNameHierarchy() : NameHierarchy(path.str(), NAME_DELIMITER_FILE));
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
	MessageFocusIn(std::vector<Id>(1, m_data->getId()), TOOLTIP_ORIGIN_GRAPH).dispatch();
}

void QtGraphNodeData::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusOut(std::vector<Id>(1, m_data->getId())).dispatch();
}
