#ifndef MESSAGE_FOCUS_IN_H
#define MESSAGE_FOCUS_IN_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageFocusIn : public Message<MessageFocusIn>
{
public:
    MessageFocusIn(Id tokenId)
        : tokenId(tokenId)
    {
    }

    static const std::string getStaticType()
    {
        return "MessageFocusIn";
    }

    const Id tokenId;
};

#endif //MESSAGE_FOCUS_IN_H
