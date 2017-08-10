#include "qt/window/QtSplashScreen.h"

#include <QApplication>
#include <QThread>
#include <QTimer>

#include "qt/utility/QtDeviceScaledPixmap.h"

#include "utility/ResourcePaths.h"

namespace
{
	class InitThread : public QThread
	{
	public:
		void run(void)
		{
			// Mininmum time the SplashScreen gets displayed.
			QThread::msleep(5000);
		}
	};
}

QtSplashScreen::QtSplashScreen(const QPixmap &pixmap, Qt::WindowFlags f)
	: QSplashScreen(pixmap, f)
	, m_state(0)
{
	QtDeviceScaledPixmap foreground((ResourcePaths::getGuiPath().str() + "splash_white.png").c_str());
	foreground.scaleToHeight(pixmap.size().height() * 0.8);
	m_foreground = foreground.pixmap();

	QtDeviceScaledPixmap background((ResourcePaths::getGuiPath().str() + "splash_blue.png").c_str());
	background.scaleToHeight(pixmap.size().height() * 0.9);
	m_background = background.pixmap();
}

QtSplashScreen::~QtSplashScreen()
{
}

void QtSplashScreen::exec(QApplication& app)
{
	m_state = 0;
	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, this, &QtSplashScreen::animate);
	timer->start(150);

	app.processEvents();

	show();
	repaint();

	app.processEvents();
}

void QtSplashScreen::setMessage(const QString &str)
{
	m_string = str;
	repaint();
}

void QtSplashScreen::setVersion(const QString &str)
{
	m_version = str;
	repaint();
}

void QtSplashScreen::animate()
{
	m_state = (m_state + 2) % 120;
	repaint();
}

void QtSplashScreen::drawContents(QPainter *painter)
{
	painter->save();
	painter->translate(rect().width() / 2, rect().height() / 2);
	painter->rotate(m_state * 3);
	painter->drawPixmap(-rect().width() * 0.9 / 2, -rect().height() * 0.9 / 2, m_background);
	painter->restore();
	painter->drawPixmap(rect().width() * 0.1, rect().height() * 0.1, m_foreground);

	QRect r = rect();
	r.setRect(r.x() + 5, r.height() - 20, r.width() - 10, 20);

	painter->drawText(r, Qt::AlignRight, QString("Sourcetrail v").append(m_version));

	// Draw message at given position, limited to 43 chars
	// If message is too long, string is truncated
	if (m_string.length() > 40)
	{
		m_string.truncate(39);
		m_string += "...";
	}

	painter->drawText(r, Qt::AlignLeft, m_string);
}

