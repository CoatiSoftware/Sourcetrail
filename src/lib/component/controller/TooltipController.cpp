#include "TooltipController.h"

#include "CodeView.h"
#include "GraphView.h"
#include "StorageAccess.h"
#include "TooltipView.h"

#include "MessageActivateSourceLocations.h"
#include "TabId.h"
#include "TaskDecoratorDelay.h"
#include "TaskLambda.h"

Id TooltipController::TooltipRequest::s_requestId = 1;

TooltipController::TooltipController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess), m_hideRequest(false)
{
}

TooltipController::~TooltipController() {}

void TooltipController::clear()
{
	{
		std::lock_guard<std::mutex> lock(m_showRequestMutex);
		m_showRequest.reset();
	}
	m_hideRequest = false;

	getView()->hideTooltip(true);
}

void TooltipController::handleMessage(MessageActivateTokens* message)
{
	clear();
}

void TooltipController::handleMessage(MessageActivateLocalSymbols* message)
{
	clear();
}

void TooltipController::handleMessage(MessageFocusIn* message)
{
	if (!message->tokenIds.size() || message->origin == TOOLTIP_ORIGIN_NONE)
	{
		return;
	}

	requestTooltipShow(message->tokenIds, TooltipInfo(), message->origin);
}

void TooltipController::handleMessage(MessageFocusOut* message)
{
	requestTooltipHide();
}

void TooltipController::handleMessage(MessageGraphNodeExpand* message)
{
	clear();
}

void TooltipController::handleMessage(MessageScrollCode* message)
{
	clear();
}

void TooltipController::handleMessage(MessageScrollGraph* message)
{
	clear();
}

void TooltipController::handleMessage(MessageTooltipHide* message)
{
	clear();
}

void TooltipController::handleMessage(MessageTooltipShow* message)
{
	if (!message->tooltipInfo.title.empty())
	{
		requestTooltipShow(std::vector<Id>(), message->tooltipInfo, message->origin);
	}
	else
	{
		TooltipInfo info = m_storageAccess->getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
			message->sourceLocationIds, message->localSymbolIds);

		// If a tooltip list would only display one token, then just activate it instead.
		// This can happen when edges pointing to the token use the same source location e.g.
		// override edges
		if (!message->force && info.snippets.size() == 1)
		{
			MessageActivateSourceLocations(message->sourceLocationIds, false).dispatch();
		}
		else if (info.snippets.size())
		{
			getView()->showTooltip(info, getViewForOrigin(message->origin));

			{
				std::lock_guard<std::mutex> lock(m_showRequestMutex);
				m_showRequest.reset();
			}
			m_hideRequest = false;
		}
	}
}

void TooltipController::handleMessage(MessageWindowFocus* message)
{
	clear();
}

TooltipView* TooltipController::getView() const
{
	return Controller::getView<TooltipView>();
}

View* TooltipController::getViewForOrigin(TooltipOrigin origin) const
{
	std::string viewName =
		(origin == TOOLTIP_ORIGIN_CODE ? CodeView::VIEW_NAME : GraphView::VIEW_NAME);
	return getView()->getViewLayout()->findFloatingView(viewName);
}

void TooltipController::requestTooltipShow(
	const std::vector<Id> tokenIds, TooltipInfo info, TooltipOrigin origin)
{
	Id requestId = TooltipRequest::s_requestId++;

	{
		std::lock_guard<std::mutex> lock(m_showRequestMutex);

		m_showRequest = std::make_unique<TooltipRequest>();
		m_showRequest->requestId = requestId;
		m_showRequest->tokenIds = tokenIds;
		m_showRequest->info = info;
		m_showRequest->origin = origin;
	}

	size_t delayMS = 700;
	if (getView()->tooltipVisible())
	{
		delayMS = 300;
	}

	Task::dispatch(
		TabId::app(),
		std::make_shared<TaskDecoratorDelay>(delayMS)->addChildTask(
			std::make_shared<TaskLambda>([requestId, this]() {
				std::unique_ptr<TooltipRequest> request;
				{
					std::lock_guard<std::mutex> lock(m_showRequestMutex);
					if (!m_showRequest || m_showRequest->requestId != requestId)
					{
						return;
					}
					request = std::move(m_showRequest);
				}

				if (!request->info.isValid() && request->tokenIds.size())
				{
					request->info = m_storageAccess->getTooltipInfoForTokenIds(
						request->tokenIds, request->origin);
				}

				if (request->info.isValid())
				{
					getView()->showTooltip(request->info, getViewForOrigin(request->origin));
					m_hideRequest = false;
				}
			})));
}

void TooltipController::requestTooltipHide()
{
	{
		std::lock_guard<std::mutex> lock(m_showRequestMutex);
		m_showRequest.reset();
	}
	m_hideRequest = true;

	Task::dispatch(
		TabId::app(),
		std::make_shared<TaskDecoratorDelay>(500)->addChildTask(std::make_shared<TaskLambda>([this]() {
			if (m_hideRequest)
			{
				m_hideRequest = false;

				getView()->hideTooltip(false);
			}
		})));
}
