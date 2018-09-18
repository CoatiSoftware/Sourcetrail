#ifndef IDE_COMMUNICATION_CONTROLLER_H
#define IDE_COMMUNICATION_CONTROLLER_H

#include <string>

#include "Controller.h"
#include "NetworkProtocolHelper.h"

#include "MessageListener.h"
#include "MessageWindowFocus.h"
#include "MessageIDECreateCDB.h"
#include "MessageMoveIDECursor.h"
#include "MessagePluginPortChange.h"

class StorageAccess;

class IDECommunicationController
	: public Controller
	, public MessageListener<MessageWindowFocus>
	, public MessageListener<MessageIDECreateCDB>
	, public MessageListener<MessageMoveIDECursor>
	, public MessageListener<MessagePluginPortChange>
{
public:
	IDECommunicationController(StorageAccess* storageAccess);
	virtual ~IDECommunicationController();

	virtual void clear();

	virtual void startListening() = 0;
	virtual void stopListening() = 0;
	virtual bool isListening() const = 0;

	void handleIncomingMessage(const std::wstring& message);

	bool getEnabled() const;
	void setEnabled(const bool enabled);

protected:
	void sendUpdatePing();

private:
	void handleSetActiveTokenMessage(const NetworkProtocolHelper::SetActiveTokenMessage& message);
	void handleCreateProjectMessage(const NetworkProtocolHelper::CreateProjectMessage& message);
	void handleCreateCDBProjectMessage(const NetworkProtocolHelper::CreateCDBProjectMessage& message);
	void handlePing(const NetworkProtocolHelper::PingMessage& message);

	virtual void handleMessage(MessageWindowFocus* message);
	virtual void handleMessage(MessageIDECreateCDB* message);
	virtual void handleMessage(MessageMoveIDECursor* message);
	virtual void handleMessage(MessagePluginPortChange* message);
	virtual void sendMessage(const std::wstring& message) const = 0;

	StorageAccess* m_storageAccess;

	bool m_enabled;
};

#endif // IDE_COMMUNICATION_CONTROLLER_H
