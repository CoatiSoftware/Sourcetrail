#ifndef TOOLTIP_CONTROLLER_H
#define TOOLTIP_CONTROLLER_H

#include "Controller.h"
#include "MessageListener.h"
#include "MessageActivateTokens.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageGraphNodeExpand.h"
#include "MessageScrollCode.h"
#include "MessageScrollGraph.h"
#include "MessageTooltipHide.h"
#include "MessageTooltipShow.h"
#include "MessageWindowFocus.h"

class StorageAccess;
class TooltipView;

class TooltipController
    : public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageFocusIn>
    , public MessageListener<MessageFocusOut>
    , public MessageListener<MessageGraphNodeExpand>
    , public MessageListener<MessageScrollCode>
    , public MessageListener<MessageScrollGraph>
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
	virtual void handleMessage(MessageActivateLocalSymbols* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageScrollGraph* message);
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
	std::mutex m_showRequestMutex;
	bool m_hideRequest;
};

#endif // TOOLTIP_CONTROLLER_H
