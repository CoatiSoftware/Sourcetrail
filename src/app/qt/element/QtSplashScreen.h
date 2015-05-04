#ifndef QTSPLASHSCREEN_H
#define QTSPLASHSCREEN_H

#include <QSplashScreen>
#include <QPainter>
#include <QWidget>

class QPixmap;

class QtSplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    QtSplashScreen(const QPixmap& pixmap, Qt::WindowFlags f = 0);
    QtSplashScreen(Qt::WindowFlags f = 0);
    virtual ~QtSplashScreen();
    void setBackground(QPixmap& pixmap);
    void setForeground(QPixmap& pixmap);
protected:
    void drawContents(QPainter* painter);

public slots:
    void animate();
    void message(const QString& str, int flag = Qt::AlignLeft, const QColor& color = Qt::black);

private:
    QString m_string;
    int state;
    QPixmap m_Background;
    QPixmap m_Foreground;
};

#endif //QTSPLASHSCREEN_H
