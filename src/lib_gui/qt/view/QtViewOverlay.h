#ifndef QT_VIEW_OVERLAY
#define QT_VIEW_OVERLAY

#include <QWidget>

#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageLoadBefore.h"
#include "utility/messaging/type/MessageLoadAfter.h"

class QTimer;

class ResizeFilter
	: public QObject
{
	Q_OBJECT

signals:
	void triggered();

public:
	ResizeFilter(QWidget* widget);

protected:
	bool eventFilter(QObject* obj, QEvent* event);

private:
	QWidget* m_widget;
};


class QtOverlay
	: public QWidget
{
	Q_OBJECT

public:
	QtOverlay(QWidget* parent = nullptr);

public slots:
	void animate();

protected:
	void paintEvent(QPaintEvent *event);

private:
	size_t m_count;
	size_t m_size;
};


class QtViewOverlay
	: public MessageListener<MessageLoadBefore>
	, public MessageListener<MessageLoadAfter>
{
public:
	QtViewOverlay(QWidget* parent = 0);

private:
	void handleMessage(MessageLoadBefore* message);
	void handleMessage(MessageLoadAfter* message);

	void doBefore();
	void doAfter();

	QWidget* m_parent;
	QtOverlay* m_overlay;

	QTimer* m_timer;

	QtThreadedFunctor<void> m_beforeFunctor;
	QtThreadedFunctor<void> m_afterFunctor;
};

#endif // QT_VIEW_OVERLAY
