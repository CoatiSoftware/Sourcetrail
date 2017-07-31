#ifndef TOOLTIP_CONTROLLER_H
#define TOOLTIP_CONTROLLER_H

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageTooltipHide.h"
#include "utility/messaging/type/MessageTooltipShow.h"
#include "utility/messaging/type/MessageWindowFocus.h"

class StorageAccess;
class TooltipView;

class TooltipController
    : public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageFocusIn>
    , public MessageListener<MessageFocusOut>
    , public MessageListener<MessageGraphNodeExpand>
    , public MessageListener<MessageTooltipHide>
    , public MessageListener<MessageTooltipShow>
    , public MessageListener<MessageWindowFocus>
{
public:
	TooltipController(StorageAccess* storageAccess);
	virtual ~TooltipController();

	// Controller
	virtual void clear();

	// MessageListener
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageTooltipHide* message);
	virtual void handleMessage(MessageTooltipShow* message);
	virtual void handleMessage(MessageWindowFocus* message);

private:
	struct TooltipRequest
	{
		static Id s_requestId;

		Id requestId;
		std::vector<Id> tokenIds;

		TooltipInfo info;
		TooltipOrigin origin;
	};

	TooltipView* getView() const;
	View* getViewForOrigin(TooltipOrigin origin) const;

	void requestTooltipShow(const std::vector<Id> tokenIds, TooltipInfo info, TooltipOrigin origin);
	void requestTooltipHide();

	StorageAccess* m_storageAccess;

	std::unique_ptr<TooltipRequest> m_showRequest;
	bool m_hideRequest;
};

#endif // TOOLTIP_CONTROLLER_H
