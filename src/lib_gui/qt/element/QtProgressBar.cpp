#include "qt/element/QtProgressBar.h"

#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

#include "utility/ResourcePaths.h"

QtProgressBar::QtProgressBar(QWidget* parent)
	: QWidget(parent)
	, m_percent(0)
	, m_count(0)
	, m_pixmap((ResourcePaths::getGuiPath() + "indexing_dialog/progress_bar_element.png").c_str())
{
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));

	m_pixmap.scaleToHeight(20);
}

void QtProgressBar::showProgress(size_t percent)
{
	m_percent = percent;

	stop();
	show();

	update();
}

void QtProgressBar::showUnknownProgressAnimated()
{
	start();
	show();
}

void QtProgressBar::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	if (m_count)
	{
		const QPixmap& pixmap = m_pixmap.pixmap();

		for (int x = -36 + (m_count % 36); x < geometry().width(); x += 18)
		{
			painter.drawPixmap(QPointF(x, -5), pixmap);
		}
	}
	else
	{
		painter.fillRect(0, 2, geometry().width() * m_percent / 100, 6, "white");
	}
}

void QtProgressBar::start()
{
	m_timePoint = TimePoint::now();
	m_timer->start(25);
}

void QtProgressBar::stop()
{
	m_timer->stop();
	m_count = 0;
}

void QtProgressBar::animate()
{
	TimePoint t = TimePoint::now();
	size_t dt = t.deltaMS(m_timePoint);

	if (dt < 5)
	{
		return;
	}

	m_timePoint = t;
	m_count++;

	update();
}
