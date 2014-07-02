#include "qt/view/graphElements/QtGraphNode.h"

#include "qgraphicsscene.h"



QtGraphNode::QtGraphNode(const Vec2i& position, const std::string& name)
{
	this->setRect(position.x, position.y, 100, 100);
	QBrush brush(Qt::lightGray);
	this->setBrush(brush);

	m_text = new QGraphicsTextItem(this);
	m_text->setPos(position.x, position.y);
	m_text->setPlainText(QString(name.c_str()));
}

QtGraphNode::~QtGraphNode()
{
}