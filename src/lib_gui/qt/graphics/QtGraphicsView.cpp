#include "qt/graphics/QtGraphicsView.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "qt/utility/QtContextMenu.h"

QtGraphicsView::QtGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
	, m_zoomFactor(1.0f)
	, m_appZoomFactor(1.0f)
	, m_up(false)
	, m_down(false)
	, m_left(false)
	, m_right(false)
	, m_shift(false)
{
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	m_timer = std::make_shared<QTimer>(this);
	connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(updateTimer()));

	m_timerStopper = std::make_shared<QTimer>(this);
	m_timerStopper->setSingleShot(true);
	connect(m_timerStopper.get(), SIGNAL(timeout()), this, SLOT(stopTimer()));

	m_exportGraphAction = new QAction(tr("Save as Image"), this);
	m_exportGraphAction->setStatusTip(tr("Save this graph as image file"));
	m_exportGraphAction->setToolTip(tr("Save this graph as image file"));
	connect(m_exportGraphAction, SIGNAL(triggered()), this, SLOT(exportGraph()));
}

float QtGraphicsView::getZoomFactor() const
{
	return m_appZoomFactor;
}

void QtGraphicsView::setAppZoomFactor(float appZoomFactor)
{
	m_appZoomFactor = appZoomFactor;
	updateTransform();
}

void QtGraphicsView::ensureVisibleAnimated(const QRectF& rect, int xmargin, int ymargin)
{
	int xval = horizontalScrollBar()->value();
	int yval = verticalScrollBar()->value();

	ensureVisible(rect, xmargin, ymargin);

	int xval2 = horizontalScrollBar()->value();
	int yval2 = verticalScrollBar()->value();

	horizontalScrollBar()->setValue(xval);
	verticalScrollBar()->setValue(yval);

	QParallelAnimationGroup* move = new QParallelAnimationGroup();

	QPropertyAnimation* xanim = new QPropertyAnimation(horizontalScrollBar(), "value");
	xanim->setDuration(150);
	xanim->setStartValue(xval);
	xanim->setEndValue(xval2);
	xanim->setEasingCurve(QEasingCurve::InOutQuad);
	move->addAnimation(xanim);

	QPropertyAnimation* yanim = new QPropertyAnimation(verticalScrollBar(), "value");
	yanim->setDuration(150);
	yanim->setStartValue(yval);
	yanim->setEndValue(yval2);
	yanim->setEasingCurve(QEasingCurve::InOutQuad);
	move->addAnimation(yanim);

	move->start();
}

void QtGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()))
	{
		m_last = event->pos();
	}

	QGraphicsView::mousePressEvent(event);
}

void QtGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()) && event->pos() == m_last)
	{
		emit emptySpaceClicked();
	}

	QGraphicsView::mouseReleaseEvent(event);
	viewport()->setCursor(Qt::ArrowCursor);
}

void QtGraphicsView::keyPressEvent(QKeyEvent* event)
{
	bool moved = moves();

	switch (event->key())
	{
		case Qt::Key_W:
			m_up = true;
			break;
		case Qt::Key_A:
			m_left = true;
			break;
		case Qt::Key_S:
			m_down = true;
			break;
		case Qt::Key_D:
			m_right = true;
			break;
		case Qt::Key_0:
			m_zoomFactor = 1.0f;
			updateTransform();
			break;
		case Qt::Key_Shift:
			m_shift = true;
			break;
		default:
			QGraphicsView::keyPressEvent(event);
			return;
	}

	if (!moved && moves())
	{
		m_timer->start(20);
	}

	m_timerStopper->start(1000);
}

void QtGraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	switch (event->key())
	{
		case Qt::Key_W:
			m_up = false;
			break;
		case Qt::Key_A:
			m_left = false;
			break;
		case Qt::Key_S:
			m_down = false;
			break;
		case Qt::Key_D:
			m_right = false;
			break;
		case Qt::Key_Shift:
			m_shift = false;
			break;
		default:
			return;
	}

	if (!moves())
	{
		m_timer->stop();
	}
}

void QtGraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() == Qt::ShiftModifier && event->delta() != 0.0f)
	{
		updateZoom(event->delta());
		return;
	}

	QGraphicsView::wheelEvent(event);
}

void QtGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu::getInstance()->showExtended(event, this, std::vector<QAction*>(1, m_exportGraphAction));
}

void QtGraphicsView::updateTimer()
{
	float ds = 30.0f;
	float dz = 50.0f;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (m_shift)
	{
		if (m_up)
		{
			z += dz;
		}
		else if (m_down)
		{
			z -= dz;
		}
	}
	else
	{
		if (m_up)
		{
			y -= ds;
		}
		else if (m_down)
		{
			y += ds;
		}

		if (m_left)
		{
			x -= ds;
		}
		else if (m_right)
		{
			x += ds;
		}
	}

	if (x != 0)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + x);
	}

	if (y != 0)
	{
		verticalScrollBar()->setValue(verticalScrollBar()->value() + y);
	}

	if (z != 0)
	{
		updateZoom(z);
	}
}

void QtGraphicsView::stopTimer()
{
	m_timer->stop();
}

void QtGraphicsView::exportGraph()
{
	QString fileName = QFileDialog::getSaveFileName(
		this, "Save image", QDir::homePath(), "PNG (*.png);;JPEG (*.JPEG);;BMP Files (*.bmp)");

	if (!fileName.isNull())
	{
		QImage image(scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		scene()->render(&painter);
		image.save(fileName);

		// different approach
		// QPixmap pixMap = grab();
		// pixMap.save(fileName);
	}
}

bool QtGraphicsView::moves() const
{
	return m_up || m_down || m_left || m_right;
}

void QtGraphicsView::updateTransform()
{
	float zoomFactor = m_appZoomFactor * m_zoomFactor;
	setTransform(QTransform(zoomFactor, 0, 0, zoomFactor, 0, 0));
}

void QtGraphicsView::updateZoom(float delta)
{
	float factor = 1.0f + 0.001 * delta;

	if (factor <= 0.0f)
	{
		factor = 0.000001;
	}

	double newZoom = m_zoomFactor * factor;
	m_zoomFactor = qBound(0.1, newZoom, 100.0);

	updateTransform();
}
