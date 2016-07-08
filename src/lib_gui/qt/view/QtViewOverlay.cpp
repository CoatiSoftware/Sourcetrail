#include "qt/view/QtViewOverlay.h"

#include <cmath>

#include <QEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

ResizeFilter::ResizeFilter(QWidget* widget)
	: m_widget(widget)
{
}

bool ResizeFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		QWidget* parent = dynamic_cast<QWidget*>(obj);
		m_widget->setGeometry(0, 0, parent->width(), parent->height());
	}

	return QObject::eventFilter(obj, event);
}

QtOverlay::QtOverlay(QWidget* parent)
	: QWidget(parent)
	, m_count(rand() % 1000000)
	, m_size(40)
{
	m_timer = new QTimer(this);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
}

void QtOverlay::start()
{
	m_timePoint = TimePoint::now();
	m_timer->start(FRAME_DELAY_MS);
	show();
}

void QtOverlay::stop()
{
	m_timer->stop();
	hide();
}

void QtOverlay::animate()
{
	TimePoint t = TimePoint::now();
	size_t dt = t.deltaMS(m_timePoint);

	if (dt < 5)
	{
		return;
	}

	m_timePoint = t;
	m_count += dt;

	QPoint center = geometry().center();
	update(QRect(center.x() - m_size / 2, center.y() - m_size / 2, m_size, m_size));
}

void QtOverlay::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.fillRect(geometry(), "#AAFFFFFF");

	QPoint center = geometry().center();

	size_t x = m_size / 2;
	size_t y = m_size / 2;
	size_t s = (m_count / 7) % 200;

	if (s < 100)
	{
		x = std::cos(3.1415f * s / 100) * x;
	}
	else
	{
		y = std::cos(3.1415f * (s - 100) / 100) * y;
	}

	painter.translate(center.x(), center.y());
	// painter.rotate(m_count / 8);

	if (s > 50 && s < 150)
	{
		painter.setBrush(QBrush("#882E3C86"));
	}
	else
	{
		painter.setBrush(QBrush("#88007AC2"));
	}

	painter.setPen(QPen(Qt::transparent));

	painter.drawEllipse(-x, -y, 2 * x, 2 * y);
}


QtViewOverlay::QtViewOverlay(QWidget* parent)
	: m_parent(parent)
	, m_showFunctor(std::bind(&QtViewOverlay::doShowOverlay, this))
	, m_hideFunctor(std::bind(&QtViewOverlay::doHideOverlay, this))
{
	m_overlay = new QtOverlay(m_parent);
	m_overlay->hide();

	m_parent->installEventFilter(new ResizeFilter(m_overlay));

	m_delayTimer = new QTimer(m_overlay);
	m_delayTimer->setSingleShot(true);
	QObject::connect(m_delayTimer, SIGNAL(timeout()), m_overlay, SLOT(start()));
}

void QtViewOverlay::showOverlay()
{
	m_showFunctor();
}

void QtViewOverlay::hideOverlay()
{
	m_hideFunctor();
}

void QtViewOverlay::doShowOverlay()
{
	m_delayTimer->start(500);
}

void QtViewOverlay::doHideOverlay()
{
	m_delayTimer->stop();
	m_overlay->stop();
}
