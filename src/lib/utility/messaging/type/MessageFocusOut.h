#ifndef MESSAGE_FOCUS_OUT_H
#define MESSAGE_FOCUS_OUT_H

#include "utility/messaging/Message.h"
#include "utility/types.h"

class MessageFocusOut : public Message<MessageFocusOut>
{
public:
    MessageFocusOut(Id tokenId)
            : tokenId(tokenId)
    {
    }

    static const std::string getStaticType()
    {
        return "MessageHoverLeave";
    }

    const Id tokenId;
};

#endif //MESSAGE_FOCUS_OUT_H
