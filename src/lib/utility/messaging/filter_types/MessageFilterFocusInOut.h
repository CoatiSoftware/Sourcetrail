#ifndef MESSAGE_FILTER_FOCUS_IN_OUT_H
#define MESSAGE_FILTER_FOCUS_IN_OUT_H

#include "MessageFilter.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"

class MessageFilterFocusInOut: public MessageFilter
{
	void filter(MessageQueue::MessageBufferType* messageBuffer) override
	{
		if (messageBuffer->size() < 2)
		{
			return;
		}

		MessageBase* message = messageBuffer->front().get();
		if (message->getType() == MessageFocusIn::getStaticType())
		{
			for (auto it = messageBuffer->begin() + 1; it != messageBuffer->end(); it++)
			{
				if ((*it)->getType() == MessageFocusOut::getStaticType() &&
					dynamic_cast<MessageFocusIn*>(message)->tokenIds ==
						dynamic_cast<MessageFocusOut*>(it->get())->tokenIds)
				{
					messageBuffer->erase(it);
					messageBuffer->pop_front();
					return;
				}
			}
		}
	}
};

#endif	  // MESSAGE_FILTER_FOCUS_IN_OUT_H
