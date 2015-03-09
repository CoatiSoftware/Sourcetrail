#include "qt/view/graphElements/QtGraphEdge.h"

#include <cmath>

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QPainter>
#include <QPen>

#include "utility/messaging/type/MessageActivateTokens.h"

#include "component/view/graphElements/GraphNode.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "qt/graphics/QtGraphicsRoundedRectItem.h"

QtStraightConnection::QtStraightConnection(Vec4i ownerRect, Vec4i targetRect, int number, QGraphicsItem* parent)
	: QGraphicsLineItem(parent)
{
	const Vec4i& o = ownerRect;
	const Vec4i& t = targetRect;

	Vec2f a((o.x + o.z) / 2, (o.y + o.w) / 2);
	Vec2f b((t.x + t.z) / 2, (t.y + t.w) / 2);

	this->setLine(a.x, a.y, b.x, b.y);
	this->setAcceptHoverEvents(true);

	Vec2f mid = (a + b) / 2;
	Vec2f u = b - a;
	float len = u.getLength();
	u.normalize();

	float alpha = atan2(u.y, u.x);

	Vec2f w;
	w.x = (o.z - o.x) / 2 * cos(alpha);
	w.y = (o.w - o.y) / 2 * sin(alpha);
	a += u * w.getLength();
	float newLen = w.getLength();

	w.x = (t.z - t.x) / 2 * cos(alpha);
	w.y = (t.w - t.y) / 2 * sin(alpha);
	b += u * -w.getLength();
	newLen -= w.getLength();

	if (newLen > 0 && newLen < len)
	{
		mid = (a + b) / 2;
	}

	m_circle = new QtGraphicsRoundedRectItem(this);
	m_circle->setRect(mid.x - 10, mid.y - 10, 20, 20);
	m_circle->setRadius(10);
	m_circle->setPen(QPen(QColor("#F8F8F8"), 2));
	m_circle->setBrush(QBrush(QColor("#FFF")));
	m_circle->setAcceptHoverEvents(true);

	QFont font;
	font.setFamily("Myriad Pro");
	font.setWeight(QFont::Normal);
	font.setPixelSize(9);

	m_number = new QGraphicsSimpleTextItem(this);
	m_number->setFont(font);

	QString numberStr = QString::number(number);
	m_number->setText(numberStr);
	m_number->setBrush(QBrush(QColor("#666")));
	m_number->setPos(
		mid.x - QFontMetrics(m_number->font()).width(numberStr) / 2,
		mid.y - QFontMetrics(m_number->font()).height() / 2
	);
}

QtStraightConnection::~QtStraightConnection()
{
}

void QtStraightConnection::setColor(QColor color)
{
	QPen p = this->pen();
	p.setColor(color);
	this->setPen(p);

	p = m_circle->pen();
	p.setColor(color);
	m_circle->setPen(p);
}

QtCorneredConnection::QtCorneredConnection(
	Vec4i ownerRect, Vec4i targetRect, Vec4i ownerParentRect, Vec4i targetParentRect, QGraphicsItem* parent
)
	: QGraphicsLineItem(parent)
	, m_ownerRect(ownerRect)
	, m_targetRect(targetRect)
	, m_ownerParentRect(ownerParentRect)
	, m_targetParentRect(targetParentRect)
	, m_closed(false)
	, m_big(false)
{
	this->setAcceptHoverEvents(true);

	m_ownerRect.x = m_ownerRect.x - 1;
	m_ownerRect.z = m_ownerRect.z + 1;
	m_targetRect.x = m_targetRect.x - 1;
	m_targetRect.z = m_targetRect.z + 1;
}

QtCorneredConnection::~QtCorneredConnection()
{
}

QPainterPath QtCorneredConnection::shape() const
{
	int w = m_big ? 10 : 5;
	QPainterPath path;
	QPolygon poly = getPath();
	for (int i = 0; i < poly.size() - 1; i++)
	{
		path.addRect(QRectF(poly.at(i), poly.at(i + 1)).normalized().adjusted(-w, -w, w, w));
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
	while (i > 1)
	{
		i--;

		QPointF a = poly.at(i);
		QPointF b = poly.at(i - 1);

		int newDir = getDirection(a, b);
		int ar = radius;
		int br = 10;

		if (i != 1)
		{
			if (dir % 2 == 1 && std::abs(a.y() - b.y()) < 2 * br)
			{
				br = std::abs(a.y() - b.y()) / 2;
			}
			else if (dir % 2 == 0 && std::abs(a.x() - b.x()) < 2 * br)
			{
				br = std::abs(a.x() - b.x()) / 2;
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

	int arrowLength = m_big ? 15 : 5;
	int arrowWidth = m_big ? 20 : 8;
	QPointF arrow = poly.at(0) + QPointF((poly.at(0).x() - poly.at(1).x() > 0 ? -1 : 1) * arrowLength, 0);

	if (m_closed)
	{
		path.lineTo(arrow);
		path.moveTo(poly.at(0));
	}
	else
	{
		path.lineTo(poly.at(0));
	}

	arrow.setY(arrow.y() - arrowWidth / 2);
	path.lineTo(arrow);
	arrow.setY(arrow.y() + arrowWidth);
	if (m_closed)
	{
		path.lineTo(arrow);
	}
	else
	{
		path.moveTo(arrow);
	}
	path.lineTo(poly.at(0));

	painter->drawPath(path);
}

void QtCorneredConnection::setClosed(bool closed)
{
	m_closed = closed;
}

void QtCorneredConnection::setBig(bool big)
{
	m_big = big;
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

	float offsetO = 17;
	float offsetT = m_big ? 29 : 17;

	QPoint tp((it ? 1 : -1) * offsetT + t[it].x, t[it].y);
	QPoint op((io ? 1 : -1) * offsetO + o[io].x, o[io].y);

	if (it != io)
	{
		if (it && tp.x() < op.x())
		{
			io = 0;
			op = QPoint(o[io].x - offsetO, o[io].y);
		}
		else if (!it && tp.x() < op.x())
		{
			it = 1;
			tp = QPoint(t[it].x + offsetT, t[it].y);
		}
		else if (io && tp.x() > op.x())
		{
			io = 1;
			op = QPoint(o[io].x + offsetO, o[io].y);
		}
		else if (!io && tp.x() > op.x())
		{
			it = 0;
			tp = QPoint(t[it].x - offsetT, t[it].y);
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

	int verticalOffset = 3;
	if (o[io].y < t[it].y)
	{
		op.setX(op.x() + verticalOffset);
		tp.setX(tp.x() + verticalOffset);
	}
	else
	{
		op.setX(op.x() - verticalOffset);
		tp.setX(tp.x() - verticalOffset);
	}

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
	this->setZValue(getZValue(false)); // Used to draw edges always on top of nodes.

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

	if (isAggregation())
	{
		m_child =
			new QtStraightConnection(owner->getBoundingRect(), target->getBoundingRect(), getAggregationCount(), this);
	}
	else
	{
		QtCorneredConnection* child = new QtCorneredConnection(
			owner->getBoundingRect(), target->getBoundingRect(),
			owner->getParentBoundingRect(), target->getParentBoundingRect(),
			this
		);

		if (isInheritance())
		{
			child->setClosed(true);
			child->setBig(true);
		}

		m_child = child;
	}

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
	case Edge::EDGE_OVERRIDE:
		color = QColor("#CC5E89");
		break;
	case Edge::EDGE_AGGREGATION:
		color = QColor("#F8F8F8");
		break;
	case Edge::EDGE_INCLUDE:
		color = QColor("#87BA50");
		break;
	default:
		color = QColor("#878787");
		break;
	}

	m_child->setPen(QPen(color, getPenWidth(), Qt::SolidLine, Qt::RoundCap));

	setIsActive(m_isActive);
}

bool QtGraphEdge::getIsActive() const
{
	return m_isActive;
}

void QtGraphEdge::setIsActive(bool isActive)
{
	m_isActive = isActive;

	if (isActive)
	{
		if (isAggregation())
		{
			dynamic_cast<QtStraightConnection*>(m_child)->setColor(QColor("#EEE"));
		}
		else
		{
			QPen p = m_child->pen();
			p.setWidthF(getPenWidth() + 1);
			m_child->setPen(p);
		}
		this->setZValue(getZValue(isActive));
	}
	else
	{
		if (isAggregation())
		{
			dynamic_cast<QtStraightConnection*>(m_child)->setColor(QColor("#F8F8F8"));
		}
		else
		{
			QPen p = m_child->pen();
			p.setWidthF(getPenWidth());
			m_child->setPen(p);
		}
		this->setZValue(getZValue(isActive));
	}
}

void QtGraphEdge::onClick()
{
	if (isAggregation())
	{
		const std::set<Id>& ids = getData()->getComponent<TokenComponentAggregation>()->getAggregationIds();
		MessageActivateTokens message(std::vector<Id>(ids.begin(), ids.end()));
		message.isAggregation = true;
		message.dispatch();
	}
	else
	{
		MessageActivateTokens message(getData()->getId());
		message.isEdge = true;
		message.dispatch();
	}
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

bool QtGraphEdge::isAggregation() const
{
	return getData()->getType() == Edge::EDGE_AGGREGATION;
}

bool QtGraphEdge::isInheritance() const
{
	return getData()->getType() == Edge::EDGE_INHERITANCE;
}

int QtGraphEdge::getZValue(bool active) const
{
	if (isAggregation())
	{
		if (active)
		{
			return -1;
		}
		return -5;
	}

	if (active)
	{
		return 5;
	}
	return 1;
}

float QtGraphEdge::getPenWidth() const
{
	if (isAggregation())
	{
		return getAggregationCount() + 1;
	}
	return 1;
}

int QtGraphEdge::getAggregationCount() const
{
	if (isAggregation())
	{
		return getData()->getComponent<TokenComponentAggregation>()->getAggregationCount();
	}
	return 0;
}
