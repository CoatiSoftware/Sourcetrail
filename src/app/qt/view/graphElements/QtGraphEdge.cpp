#include "qt/view/graphElements/QtGraphEdge.h"

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QPen>

#include "component/view/graphElements/GraphNode.h"


QtStraightConnection::QtStraightConnection(Vec4i ownerRect, Vec4i targetRect, QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
{
	const Vec4i& o = ownerRect;
	const Vec4i& t = targetRect;

	Vec2f po[2] = { Vec2f(o.x, (o.y + 2 * o.w) / 3), Vec2f(o.z, (o.y + 2 * o.w) / 3) };
	Vec2f pt[2] = { Vec2f(t.x, (2 * t.y + t.w) / 3), Vec2f(t.z, (2 * t.y + t.w) / 3) };

	int io = -1;
	int it = -1;
	float dist = -1;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Vec2f diff = po[i] - pt[j];
			if (dist < 0 || diff.getLength() < dist)
			{
				dist = diff.getLength();
				io = i;
				it = j;
			}
		}
	}

	this->setLine(po[io].x, po[io].y, pt[it].x, pt[it].y);
	this->setAcceptHoverEvents(true);
}

QtStraightConnection::~QtStraightConnection()
{
}

QPainterPath QtStraightConnection::shape() const
{
	QPainterPath path;
	QLineF l = line();
	QLineF n = l.normalVector();
	n.setLength(5.0f);

	qreal x = n.x2() - n.x1();
	qreal y = n.y2() - n.y1();

	path.moveTo(l.x1() + x, l.y1() + y);
	path.lineTo(l.x2() + x, l.y2() + y);
	path.lineTo(l.x2() - x, l.y2() - y);
	path.lineTo(l.x1() - x, l.y1() - y);
	path.closeSubpath();

	return path;
}


QtCorneredConnection::QtCorneredConnection(
	Vec4i ownerRect, Vec4i targetRect, Vec4i ownerParentRect, Vec4i targetParentRect, QGraphicsItem* parent
)
	: QGraphicsLineItem(parent)
	, m_ownerRect(ownerRect)
	, m_targetRect(targetRect)
	, m_ownerParentRect(ownerParentRect)
	, m_targetParentRect(targetParentRect)
{
	this->setAcceptHoverEvents(true);
}

QtCorneredConnection::~QtCorneredConnection()
{
}

QPainterPath QtCorneredConnection::shape() const
{
	QPainterPath path;
	QPolygon poly = getPath();
	for (int i = 0; i < poly.size() - 1; i++)
	{
		path.addRect(QRectF(poly.at(i), poly.at(i + 1)).normalized().adjusted(-5, -5, 5, 5));
	}
	return path;
}

void QtCorneredConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget)
{
	painter->setPen(pen());

	QPainterPath path;

	QPolygon poly = getPath();
	int i = poly.length() - 1;

	path.moveTo(poly.at(i));

	int radius = 10;
	int dir = getDirection(poly.at(i), poly.at(i - 1));
	while (i > 0)
	{
		i--;
		if (i == 0)
		{
			path.lineTo(poly.at(i));
		}
		else
		{
			QPointF a = poly.at(i);
			QPointF b = poly.at(i - 1);

			int newDir = getDirection(a, b);
			int ar = radius;
			int br = 10;

			if (i != 1)
			{
				if (dir % 2 == 1 && abs(a.y() - b.y()) < 2 * br)
				{
					br = abs(a.y() - b.y()) / 2;
				}
				else if (dir % 2 == 0 && abs(a.x() - b.x()) < 2 * br)
				{
					br = abs(a.x() - b.x()) / 2;
				}
			}

			switch (dir)
			{
			case 0: a.setY(a.y() + ar); break;
			case 1: a.setX(a.x() - ar); break;
			case 2: a.setY(a.y() - ar); break;
			case 3: a.setX(a.x() + ar); break;
			}

			b = poly.at(i);
			switch (newDir)
			{
			case 0: b.setY(b.y() - br); break;
			case 1: b.setX(b.x() + br); break;
			case 2: b.setY(b.y() + br); break;
			case 3: b.setX(b.x() - br); break;
			}

			switch (dir)
			{
			case 0:
				if (newDir == 1)
				{
					path.arcTo(a.x(), b.y(), 2 * br, 2 * ar, 180, -90);
				}
				else if (newDir == 3)
				{
					path.arcTo(b.x() - br, a.y() - ar, 2 * br, 2 * ar, 0, 90);
				}
				break;
			case 1:
				if (newDir == 0)
				{
					path.arcTo(a.x() - ar, b.y() - br, 2 * ar, 2 * br, -90, 90);
				}
				else if (newDir == 2)
				{
					path.arcTo(a.x() - ar, a.y(), 2 * ar, 2 * br, 90, -90);
				}
				break;
			case 2:
				if (newDir == 1)
				{
					path.arcTo(a.x(), a.y() - ar, 2 * br, 2 * ar, 180, 90);
				}
				else if (newDir == 3)
				{
					path.arcTo(b.x() - br, a.y() - ar, 2 * br, 2 * ar, 0, -90);
				}
				break;
			case 3:
				if (newDir == 0)
				{
					path.arcTo(b.x(), b.y() - br, 2 * ar, 2 * br, -90, -90);
				}
				else if (newDir == 2)
				{
					path.arcTo(b.x(), a.y(), 2 * ar, 2 * br, 90, 90);
				}
				break;
			}

			dir = newDir;
			radius = br;
		}
	}

	int arrowLength = 3;
	int arrowWidth = 6;

	QPointF arrow = poly.at(0) + QPointF((poly.at(0).x() - poly.at(1).x() > 0 ? -1 : 1) * arrowLength, -arrowWidth / 2);
	path.lineTo(arrow);
	arrow.setY(arrow.y() + arrowWidth);
	path.moveTo(arrow);
	path.lineTo(poly.at(0));

	painter->drawPath(path);
}

QPolygon QtCorneredConnection::getPath() const
{
	const Vec4i& oR = m_ownerRect;
	const Vec4i& tR = m_targetRect;

	Vec2f o[2] = { Vec2f(m_ownerParentRect.x, (oR.y + 2 * oR.w) / 3), Vec2f(m_ownerParentRect.z, (oR.y + 2 * oR.w) / 3) };
	Vec2f t[2] = { Vec2f(m_targetParentRect.x, (2 * tR.y + tR.w) / 3), Vec2f(m_targetParentRect.z, (2 * tR.y + tR.w) / 3) };

	int io = -1;
	int it = -1;
	float dist = -1;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Vec2f diff = o[i] - t[j];
			if (dist < 0 || diff.getLength() < dist)
			{
				dist = diff.getLength();
				io = i;
				it = j;
			}
		}
	}

	float offset = 15;

	QPoint tp((it ? 1 : -1) * offset + t[it].x, t[it].y);
	QPoint op((io ? 1 : -1) * offset + o[io].x, o[io].y);

	if (it != io)
	{
		if (it && tp.x() < op.x())
		{
			io = 0;
			op = QPoint(o[io].x - offset, o[io].y);
		}
		else if (!it && tp.x() < op.x())
		{
			it = 1;
			tp = QPoint(t[it].x + offset, t[it].y);
		}
		else if (io && tp.x() > op.x())
		{
			io = 1;
			op = QPoint(o[io].x + offset, o[io].y);
		}
		else if (!io && tp.x() > op.x())
		{
			it = 0;
			tp = QPoint(t[it].x - offset, t[it].y);
		}
	}

	if (it == io && ((it && tp.x() < op.x()) || (!it && tp.x() > op.x())) )
	{
		tp.setX(op.x());
	}
	else
	{
		op.setX(tp.x());
	}

	o[0] = Vec2f(oR.x, (oR.y + 2 * oR.w) / 3);
	o[1] = Vec2f(oR.z, (oR.y + 2 * oR.w) / 3);
	t[0] = Vec2f(tR.x, (2 * tR.y + tR.w) / 3);
	t[1] = Vec2f(tR.z, (2 * tR.y + tR.w) / 3);

	QPolygon poly;
	poly << QPoint(t[it].x, t[it].y);
	poly << tp;
	poly << op;
	poly << QPoint(o[io].x, o[io].y);
	return poly;
}

int QtCorneredConnection::getDirection(const QPointF& a, const QPointF& b) const
{
	if (a.x() != b.x())
	{
		if (a.x() < b.x())
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	else
	{
		if (a.y() < b.y())
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
}


QtGraphEdge::QtGraphEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target, const Edge* data)
	: GraphEdge(data)
	, m_owner(owner)
	, m_target(target)
	, m_child(nullptr)
	, m_isActive(false)
	, m_mousePos(0.0f, 0.0f)
	, m_mouseMoved(false)
{
	this->setAcceptHoverEvents(true);
	this->setZValue(1); // Used to draw edges always on top of nodes.

	this->updateLine();
}

QtGraphEdge::~QtGraphEdge()
{
}

std::weak_ptr<GraphNode> QtGraphEdge::getOwner()
{
	return m_owner;
}

std::weak_ptr<GraphNode> QtGraphEdge::getTarget()
{
	return m_target;
}

void QtGraphEdge::updateLine()
{
	std::shared_ptr<GraphNode> owner = m_owner.lock();
	std::shared_ptr<GraphNode> target = m_target.lock();

	if (owner == NULL || target == NULL)
	{
		LOG_WARNING("Either the owner or the target node is null.");
		return;
	}

	if (m_child)
	{
		delete m_child;
	}

	m_child = new QtCorneredConnection(
		owner->getBoundingRect(), target->getBoundingRect(),
		owner->getParentBoundingRect(), target->getParentBoundingRect(),
		this
	);

	QColor color;

	switch (getData()->getType())
	{
	case Edge::EDGE_CALL:
		color = QColor("#F1C100");
		break;
	case Edge::EDGE_USAGE:
		color = QColor("#62B29D");
		break;
	case Edge::EDGE_INHERITANCE:
		color = QColor("#CC5E89");
		break;
	default:
		color = QColor("#878787");
		break;
	}

	m_child->setPen(QPen(color, 1));
}

bool QtGraphEdge::getIsActive() const
{
	return m_isActive;
}

void QtGraphEdge::setIsActive(bool isActive)
{
	m_isActive = isActive;

	QPen p = m_child->pen();
	if (isActive)
	{
		p.setWidth(2);
		this->setZValue(5);
	}
	else
	{
		p.setWidth(1);
		this->setZValue(1);
	}
	m_child->setPen(p);
}

void QtGraphEdge::onClick()
{
	MessageActivateToken(getData()->getId()).dispatch();
}

void QtGraphEdge::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	m_mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());
	m_mouseMoved = false;
}

void QtGraphEdge::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	Vec2i mousePos = Vec2i(event->scenePos().x(), event->scenePos().y());

	if ((mousePos - m_mousePos).getLength() > 1.0f)
	{
		m_mouseMoved = true;
	}
}

void QtGraphEdge::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_mouseMoved)
	{
		this->onClick();
	}
}

void QtGraphEdge::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	bool isActive = m_isActive;
	this->setIsActive(true);
	m_isActive = isActive;
}

void QtGraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	this->setIsActive(m_isActive);
}
