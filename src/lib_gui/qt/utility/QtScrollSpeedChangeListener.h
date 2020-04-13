#ifndef QT_SCROLL_SPEED_CHANGE_LISTENER
#define QT_SCROLL_SPEED_CHANGE_LISTENER

#include "../../../lib/utility/messaging/MessageListener.h"
#include "../../../lib/utility/messaging/type/MessageScrollSpeedChange.h"

#include "QtThreadedFunctor.h"

class QScrollBar;

class QtScrollSpeedChangeListener: public MessageListener<MessageScrollSpeedChange>
{
public:
	QtScrollSpeedChangeListener();

	void setScrollBar(QScrollBar* scrollbar);

private:
	void handleMessage(MessageScrollSpeedChange* message);

	void doChangeScrollSpeed(float scrollSpeed);

	QtThreadedFunctor<float> m_changeScrollSpeedFunctor;

	QScrollBar* m_scrollBar;
	int m_singleStep;
};

#endif	  // QT_SCROLL_SPEED_CHANGE_LISTENER
