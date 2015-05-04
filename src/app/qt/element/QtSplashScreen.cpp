#include "QtSplashScreen.h"

#include <QTimer>

QtSplashScreen::QtSplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen (pixmap, f)
{
    state = 0;
    QTimer *timer = new QTimer( this );
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(150);
    show();
    repaint();
}

QtSplashScreen::~QtSplashScreen()
{
}

void QtSplashScreen::animate()
{
    state = (state+2)%120;
    repaint();
}

void QtSplashScreen::setBackground(QPixmap& pixmap)
{
    m_Background = pixmap;
}

void QtSplashScreen::setForeground(QPixmap &pixmap)
{
    m_Foreground = pixmap;
}

void QtSplashScreen::message(const QString &str, int flag, const QColor &color)
{
    QSplashScreen::showMessage(str,flag,color);
    animate();
    m_string = str;
}

void QtSplashScreen::drawContents(QPainter *painter)
{
    painter->save();
    painter->translate(rect().width()/2,rect().height()/2);
    painter->rotate(state*3);
    painter->drawPixmap(-rect().width()*0.9/2,-rect().height()*0.9/2,m_Background);
    painter->restore();
    painter->drawPixmap(rect().width()*0.1,rect().height()*0.1,m_Foreground);

    painter->setPen(QColor(74,112,18));
    QRect r = rect();
    r.setRect(r.x() + 5, r.y() + 5, r.width() - 10, r.height() - 10);

    // TODO: automated Version number
    painter->drawText(r, Qt::AlignRight, "Version 0.0.0");

    // Draw message at given position, limited to 43 chars
    // If message is too long, string is truncated
    if (m_string.length() > 40) {m_string.truncate(39); m_string += "...";}
    painter->drawText(90, 16,m_string);
}

