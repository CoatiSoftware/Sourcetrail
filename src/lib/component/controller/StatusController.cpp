#include "component/controller/StatusController.h"

#include "data/access/StorageAccess.h"
#include "utility/logging/LogManager.h"
#include "settings/ApplicationSettings.h"
#include "component/view/StatusView.h"

StatusController::StatusController()
	: m_enabled(false)
	, m_lastSyncedStatus(0)
	, m_waiting(false)
{
}

StatusController::~StatusController()
{
}

void StatusController::setEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool StatusController::getEnabled() const
{
	return m_enabled;
}

StatusView* StatusController::getView() const
{
	return Controller::getView<StatusView>();
}

void StatusController::clear()
{
	std::lock_guard<std::mutex> lock(m_statusMutex);
	m_status.clear();
	clearTable();
	m_lastSyncedStatus = 0;
}


void StatusController::handleMessage(MessageStatus* message)
{
	std::lock_guard<std::mutex> lock(m_statusMutex);
	m_status.push_back(Status(message->status, message->isError));

	if (!m_waiting)
	{
		m_waiting = true;
		std::thread([&]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				syncStatus();
				m_waiting = false;
			}
		).detach();
	}
}

void StatusController::handleMessage(MessageStatusFilterChanged* message)
{
	m_statusFilter = message->statusFilter;
	ApplicationSettings* settings = ApplicationSettings::getInstance().get();
	settings->setStatusFilter(m_statusFilter);
	settings->save();
	clearTable();
	syncStatus();
}

void StatusController::clearTable()
{
	getView()->clear();
}

void StatusController::handleMessage(MessageClearStatusView* message)
{
	clear();
}

void StatusController::syncStatus()
{
	std::lock_guard<std::mutex> lock(m_statusMutex);

	std::vector<Status> status;

	for (size_t i = m_lastSyncedStatus; i < m_status.size(); i++)
	{
		if ((m_status[i].isError && (STATUSTYPE::STATUS_ERROR & m_statusFilter))
			|| (!m_status[i].isError && (STATUSTYPE::STATUS_INFO & m_statusFilter)))
		{
			status.push_back(m_status[i]);
		}
		m_lastSyncedStatus = i;
	}
	m_lastSyncedStatus++;

	//for (Status  s : m_status)
	//{
		//if ((s.isError && (STATUSTYPE::STATUS_ERROR & m_statusFilter))
			//|| (!s.isError && (STATUSTYPE::STATUS_INFO & m_statusFilter)))
		//{
			//status.push_back(s);
		//}
	//}

	getView()->addStatus(status);
}

