#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <deque>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageActivateTokenIds.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/messaging/type/MessageUndo.h"

#include "component/controller/helper/ActivationTranslator.h"
#include "component/controller/Controller.h"

class StorageAccess;
class UndoRedoView;

class UndoRedoController
	: public Controller
	, public MessageListener<MessageActivateEdge>
	, public MessageListener<MessageActivateFile>
	, public MessageListener<MessageActivateNodes>
	, public MessageListener<MessageActivateTokenIds>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRedo>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageShowScope>
	, public MessageListener<MessageUndo>
{
public:
	UndoRedoController(StorageAccess* storageAccess);
	virtual ~UndoRedoController();

	UndoRedoView* getView();

private:
	struct Command
	{
		Command(std::shared_ptr<MessageBase> message, size_t order);

		std::shared_ptr<MessageBase> message;
		std::shared_ptr<MessageBase> subMessage;
		size_t order;
	};

	virtual void handleMessage(MessageActivateEdge* message);
	virtual void handleMessage(MessageActivateFile* message);
	virtual void handleMessage(MessageActivateNodes* message);
	virtual void handleMessage(MessageActivateTokenIds* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageDeactivateEdge* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageRedo* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageShowScope* message);
	virtual void handleMessage(MessageUndo* message);

	void replayCommands(bool removeLast);

	void processCommand(Command command);
	void processNormalCommand(const Command& command);
	void processRedoCommand(const Command& command);
	void processUndoCommand(const Command& command);

	void clear();

	bool requiresActivateFallbackToken() const;
	bool checkCommandCausesTokenActivation(const Command& command) const;

	ActivationTranslator m_activationTranslator;
	StorageAccess* m_storageAccess;

	Command m_lastCommand;

	std::deque<Command> m_undo;
	std::deque<Command> m_redo;
};

#endif // UNDO_REDO_CONTROLLER_H
