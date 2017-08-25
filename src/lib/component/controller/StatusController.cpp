#include "component/controller/StatusController.h"

#include "settings/ApplicationSettings.h"
#include "component/view/StatusView.h"
#include "utility/utility.h"

StatusController::StatusController()
{
	m_statusFilter = ApplicationSettings::getInstance()->getStatusFilter();
}

StatusController::~StatusController()
{
}

StatusView* StatusController::getView() const
{
	return Controller::getView<StatusView>();
}

void StatusController::clear()
{
}

void StatusController::handleMessage(MessageClearStatusView* message)
{
	m_status.clear();
	getView()->clear();
}

void StatusController::handleMessage(MessageShowStatus* message)
{
	getView()->showDockWidget();
}

void StatusController::handleMessage(MessageStatus* message)
{
	if (!message->status().size())
	{
		return;
	}

	std::vector<Status> stati;

	for (const std::string& status : message->stati())
	{
		stati.push_back(Status(status, message->isError));
	}

	utility::append(m_status, stati);

	addStatus(stati);
}

void StatusController::handleMessage(MessageStatusFilterChanged* message)
{
	m_statusFilter = message->statusFilter;

	getView()->clear();
	addStatus(m_status);

	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setStatusFilter(m_statusFilter);
	settings->save();
}

void StatusController::addStatus(const std::vector<Status> status)
{
	std::vector<Status> filteredStatus;

	for (const Status& s : status)
	{
		if (s.type & m_statusFilter)
		{
			filteredStatus.push_back(s);
		}
	}

	getView()->addStatus(filteredStatus);
}
