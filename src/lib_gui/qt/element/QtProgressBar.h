#ifndef QT_PROGRESS_BAR_H
#define QT_PROGRESS_BAR_H

#include <QWidget>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "utility/TimeStamp.h"

class QTimer;

class QtProgressBar
	: public QWidget
{
	Q_OBJECT

public:
	QtProgressBar(QWidget* parent = nullptr);

	void showProgress(size_t percent);
	size_t getProgress() const;

	void showUnknownProgressAnimated();

protected:
	void paintEvent(QPaintEvent* event);

private slots:
	void start();
	void stop();

	void animate();

private:
	size_t m_percent;

	size_t m_count;
	QTimer* m_timer;
	TimeStamp m_TimeStamp;

	QtDeviceScaledPixmap m_pixmap;
};

#endif // QT_PROGRESS_BAR_H
