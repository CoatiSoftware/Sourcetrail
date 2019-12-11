#include "QtProgressBar.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

#include "ResourcePaths.h"

QtProgressBar::QtProgressBar(QWidget* parent)
	: QWidget(parent)
	, m_percent(0)
	, m_count(0)
	, m_pixmap(QString::fromStdWString(
		  ResourcePaths::getGuiPath().concatenate(L"indexing_dialog/progress_bar_element.png").wstr()))
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &QtProgressBar::animate);

	m_pixmap.scaleToHeight(20);
}

void QtProgressBar::showProgress(size_t percent)
{
	m_percent = percent;

	stop();
	show();

	update();
}

size_t QtProgressBar::getProgress() const
{
	return m_percent;
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
		painter.fillRect(0, 2, static_cast<int>(geometry().width() * m_percent / 100), 6, "white");
	}
}

void QtProgressBar::start()
{
	m_TimeStamp = TimeStamp::now();
	m_timer->start(25);
}

void QtProgressBar::stop()
{
	m_timer->stop();
	m_count = 0;
}

void QtProgressBar::animate()
{
	TimeStamp t = TimeStamp::now();
	size_t dt = t.deltaMS(m_TimeStamp);

	if (dt < 5)
	{
		return;
	}

	m_TimeStamp = t;
	m_count++;

	update();
}
