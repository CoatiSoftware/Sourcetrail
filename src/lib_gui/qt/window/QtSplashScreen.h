#ifndef QTSPLASHSCREEN_H
#define QTSPLASHSCREEN_H

#include <QSplashScreen>
#include <QPainter>
#include <QWidget>

class QCoreApplication;
class QPixmap;

class QtSplashScreen
	: public QSplashScreen
{
	Q_OBJECT

public:
	QtSplashScreen(const QPixmap& pixmap, Qt::WindowFlags f = 0);
	virtual ~QtSplashScreen();

	void exec(QCoreApplication& app);
	void setMessage(const QString& str);
	void setVersion(const QString& str);

public slots:
	void animate();

private:
	void drawContents(QPainter* painter);

	int m_state;

	QString m_string;
	QString m_version;

	QPixmap m_background;
	QPixmap m_foreground;
};

#endif //QTSPLASHSCREEN_H
