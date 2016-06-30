#ifndef QT_VIEW_OVERLAY
#define QT_VIEW_OVERLAY

#include <QObject>

#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageLoadBefore.h"
#include "utility/messaging/type/MessageLoadAfter.h"

class QWidget;

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
	QWidget* m_overlay;

	QtThreadedFunctor<void> m_beforeFunctor;
	QtThreadedFunctor<void> m_afterFunctor;
};

#endif // QT_VIEW_OVERLAY
